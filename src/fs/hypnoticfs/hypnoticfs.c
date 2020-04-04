//
// HypnoticOS
// Copyright (C) 2019, 2020  jk30
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <hypnoticos/fs.h>
#include <hypnoticos/devices/storage.h>
#include <hypnoticos/devices/storage/memory-disk.h>
#include <hypnoticos/fs/hypnoticfs.h>
#include <hypnoticos/hypnoticos.h>
#include <string.h>
#include <stdlib.h>

/**
 * Get the size of the index space.
 * @param  root The FS root.
 * @return      The index space size, or 0 on error.
 */
uint64_t Fs_HypnoticFS_GetIndexSize(FsRoot_t *root) {
  uint64_t index_size;

  if(!StorageRead(root, MAGIC_SIZE, sizeof(uint64_t), (void *) (&index_size))) {
    WARNING();
    return 0;
  } else {
    return index_size;
  }
}

/**
 * Get the index entry for a path.
 * @param  root       The FS root.
 * @param  path_short An NULL terminated array containing each part of the path.
 * @param  index      The address where the index can be placed.
 * @return            The offset where the index entry was found within the index space, or 0 if there was an error.
 */
uint64_t Fs_HypnoticFS_GetIndex(FsRoot_t *root, char **path_short, HypnoticFS_Index_t *index) {
  uint64_t i, parent_offset, index_size, filename_index;
  HypnoticFS_Index_t entry;
  uint64_t index_data_offset;

  if(path_short[0] == NULL) {
    return 0;
  }

  if((index_size = Fs_HypnoticFS_GetIndexSize(root)) < INDEX_START) {
    WARNING();
    return 0;
  }

  // Find the parent's offset
  parent_offset = 0;
  for(i = 0; path_short[i + 1] != NULL; i++) {
    uint8_t found_parent = 0;

    if(strlen(path_short[i]) > HYPNOTICFS_FILENAME_SIZE) {
      return 0;
    }

    for(uint64_t entry_offset = INDEX_START;
     entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
     entry_offset += sizeof(HypnoticFS_Index_t)) {
      if(!StorageRead(root, entry_offset, sizeof(HypnoticFS_Index_t), &entry)) {
        WARNING();
        return 0;
      }

      if(entry.type == HYPNOTICFS_TYPE_EMPTY) {
        continue;
      }

      if(entry.type != HYPNOTICFS_TYPE_DIRECTORY || entry.type == HYPNOTICFS_TYPE_EMPTY) {
        continue;
      }

      char buffer[HYPNOTICFS_FILENAME_SIZE];
      memset(buffer, 0, HYPNOTICFS_FILENAME_SIZE);
      memcpy(buffer, path_short[i], strlen(path_short[i]));
      if(memcmp(entry.name, buffer, HYPNOTICFS_FILENAME_SIZE) == 0 && entry.parent_offset == parent_offset) {
        parent_offset = entry_offset;
        found_parent = 1;
        break;
      }
    }
    if(!found_parent) {
      return 0;
    }
  }

  // Find files with the file name and the parent offset
  for(filename_index = 0; path_short[filename_index + 1] != NULL; filename_index++);
  if(strlen(path_short[filename_index]) > HYPNOTICFS_FILENAME_SIZE) {
    return 0;
  }
  uint8_t found_file = 0;
  char buffer[HYPNOTICFS_FILENAME_SIZE];
  memset(buffer, 0, HYPNOTICFS_FILENAME_SIZE);
  memcpy(buffer, path_short[filename_index], strlen(path_short[filename_index]));
  for(uint64_t entry_offset = INDEX_START;
   entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
   entry_offset += sizeof(HypnoticFS_Index_t)) {
    if(!StorageRead(root, entry_offset, sizeof(HypnoticFS_Index_t), &entry)) {
      WARNING();
      return 0;
    }

    if(entry.type != HYPNOTICFS_TYPE_EMPTY) {
      if(entry.parent_offset == parent_offset && memcmp(entry.name, buffer, HYPNOTICFS_FILENAME_SIZE) == 0) {
        found_file = 1;
        index_data_offset = entry_offset;
        break;
      }
    }
  }
  if(!found_file) {
    return 0;
  }

  // Prepare details about this file
  memcpy(index, &entry, sizeof(HypnoticFS_Index_t));

  // Return the details
  return index_data_offset;
}

/**
 * Find an empty index entry in the index space.
 * @param  root The FS root.
 * @return      The offset of the index entry, or 0 if there was an error.
 */
uint64_t Fs_HypnoticFS_GetEmptyIndex(FsRoot_t *root) {
  uint64_t index_size;
  HypnoticFS_Index_t entry;

  if((index_size = Fs_HypnoticFS_GetIndexSize(root)) < INDEX_START) {
    WARNING();
    return 0;
  }

  for(uint64_t entry_offset = INDEX_START;
   entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
   entry_offset += sizeof(HypnoticFS_Index_t)) {
    if(!StorageRead(root, entry_offset, sizeof(HypnoticFS_Index_t), &entry)) {
      WARNING();
      return 0;
    }

    if(entry.type == HYPNOTICFS_TYPE_EMPTY) {
      return entry_offset;
    }
  }

  // No empty indices found
  return 0;
}

/**
 * Find empty space in the data part of the FS.
 * @param  root The FS root.
 * @param  size The amount of space needed.
 * @return      The offset of the space found, or 0 on error.
 */
uint64_t Fs_HypnoticFS_FindSpace(FsRoot_t *root, uint64_t size) {
  uint64_t index_size, root_max;
  HypnoticFS_Index_t entry_pass1, entry_pass2;
  uint8_t all_are_empty = 1;

  if((index_size = Fs_HypnoticFS_GetIndexSize(root)) < INDEX_START) {
    WARNING();
    return 0;
  }

  root_max = StorageGetMax(root);

  // First pass
  for(uint64_t entry_offset_pass1 = INDEX_START;
   entry_offset_pass1 + sizeof(HypnoticFS_Index_t) < index_size;
   entry_offset_pass1 += sizeof(HypnoticFS_Index_t)) {
    if(!StorageRead(root, entry_offset_pass1, sizeof(HypnoticFS_Index_t), &entry_pass1)) {
      WARNING();
      return 0;
    }

    // Is this entry empty?
    if(entry_pass1.type == HYPNOTICFS_TYPE_EMPTY) {
      continue;
    }

    // This entry is not empty
    all_are_empty = 0;

    // Check if there is space before this entry
    // Check that there is space between the start of the data and this entry
    if(entry_pass1.offset - index_size > size) {
      uint64_t check_at = entry_pass1.offset - size;
      uint8_t problem = 0;
      for(uint64_t entry_offset_pass2 = INDEX_START;
       entry_offset_pass2 + sizeof(HypnoticFS_Index_t) < index_size;
       entry_offset_pass2 += sizeof(HypnoticFS_Index_t)) {
         if(!StorageRead(root, entry_offset_pass2, sizeof(HypnoticFS_Index_t), &entry_pass2)) {
           WARNING();
           return 0;
         }

         // Is this entry empty?
         if(entry_pass2.type == HYPNOTICFS_TYPE_EMPTY) {
           continue;
         }

         // Does this entry overlap with check_at?
         if(entry_pass2.offset == check_at) {
           // This entry is at the same offset
           problem = 1;
           break;
         } else if(entry_pass2.offset < check_at && entry_pass2.offset + entry_pass2.size > check_at) {
           // Overlap
           problem = 1;
           break;
         } else if(entry_pass2.offset > check_at && entry_pass2.offset + entry_pass2.size <= check_at) {
           // Overlap
           problem = 1;
           break;
         }
      }

      if(!problem) {
        // Offset does not overlap
        return check_at;
      }
    }

    // Check if there is space after this entry
    if(entry_pass1.offset + entry_pass1.size + size < root_max) {
      uint64_t check_at = entry_pass1.offset + entry_pass1.size + size;
      uint8_t problem = 0;
      for(uint64_t entry_offset_pass2 = INDEX_START;
       entry_offset_pass2 + sizeof(HypnoticFS_Index_t) < index_size;
       entry_offset_pass2 += sizeof(HypnoticFS_Index_t)) {
         if(!StorageRead(root, entry_offset_pass2, sizeof(HypnoticFS_Index_t), &entry_pass2)) {
           WARNING();
           return 0;
         }

         // Is this entry empty?
         if(entry_pass2.type == HYPNOTICFS_TYPE_EMPTY) {
           continue;
         }

         // Does this entry overlap with check_at?
         if(entry_pass2.offset == check_at) {
           // This entry is at the same offset
           problem = 1;
           break;
         } else if(entry_pass2.offset < check_at && entry_pass2.offset + entry_pass2.size > check_at) {
           // Overlap
           problem = 1;
           break;
         } else if(entry_pass2.offset > check_at && entry_pass2.offset + entry_pass2.size <= check_at) {
           // Overlap
           problem = 1;
           break;
         }
      }

      if(!problem) {
        // Offset does not overlap
        return check_at;
      }
    }
  }

  // Every index searched
  if(all_are_empty) {
    // Check if there is space between the start and end of the data
    if(root_max - index_size > size) {
      // Offset is at the start
      return index_size;
    } else {
      // No space
      WARNING();
      return 0;
    }
  } else {
    // No space
    WARNING();
    return 0;
  }
}

/**
 * Resize an index entry to a given size. The index entry provided will also be updated and the FS will be updated with that updated index entry.
 * @param  root              The FS root.
 * @param  index_data_offset The offset of the index entry in the index space.
 * @param  index             The index entry.
 * @param  new_size          The new size.
 * @return                   0 on failure, 1 on success.
 */
uint64_t Fs_HypnoticFS_Resize(FsRoot_t *root, uint64_t index_data_offset, HypnoticFS_Index_t *index, uint64_t new_size) {
  uint64_t index_size, additional_space, new_data_offset, root_max, current_data_offset;
  HypnoticFS_Index_t entry;

  if((index_size = Fs_HypnoticFS_GetIndexSize(root)) < INDEX_START) {
    WARNING();
    return 0;
  }

  current_data_offset = index->offset;

  root_max = StorageGetMax(root);

  // Is the entry being made smaller?
  if(index->size > new_size) {
    index->size = new_size;
    if(!Fs_HypnoticFS_UpdateIndex(root, index_data_offset, index)) {
      WARNING();
      return 0;
    } else {
      return 1;
    }
  }

  // More space is required
  additional_space = new_size - index->size;

  // Check if there is space after this entry
  if(current_data_offset + index->size + additional_space < root_max) {
    uint64_t check_at = current_data_offset + index->size + additional_space;
    uint8_t problem = 0;
    for(uint64_t entry_offset = INDEX_START;
     entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
     entry_offset += sizeof(HypnoticFS_Index_t)) {
       if(!StorageRead(root, entry_offset, sizeof(HypnoticFS_Index_t), &entry)) {
         WARNING();
         return 0;
       }

       // Is this entry empty?
       if(entry.type == HYPNOTICFS_TYPE_EMPTY) {
         continue;
       }

       // Is this the current entry?
       if(entry.offset == current_data_offset) {
         continue;
       }

       // Does this entry overlap with check_at?
       if(entry.offset == check_at) {
         // This entry is at the same offset
         problem = 1;
         break;
       } else if(entry.offset < check_at && entry.offset + entry.size >= check_at) {
         // Overlap
         problem = 1;
         break;
       } else if(entry.offset > check_at && entry.offset + entry.size <= check_at) {
         // Overlap
         problem = 1;
         break;
       }
    }

    if(!problem) {
      // Offset does not overlap
      index->size = new_size;
      if(!Fs_HypnoticFS_UpdateIndex(root, index_data_offset, index)) {
        WARNING();
        return 0;
      } else {
        return 1;
      }
    }
  }

  // Check if there is space before this entry
  // Check that there is space between the start of the data and this entry
  if(index_size + additional_space < current_data_offset) {
    uint64_t check_at = current_data_offset - additional_space;
    uint8_t problem = 0;
    for(uint64_t entry_offset = INDEX_START;
     entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
     entry_offset += sizeof(HypnoticFS_Index_t)) {
       if(!StorageRead(root, entry_offset, sizeof(HypnoticFS_Index_t), &entry)) {
         WARNING();
         return 0;
       }

       // Is this entry empty?
       if(entry.type == HYPNOTICFS_TYPE_EMPTY) {
         continue;
       }

       // Is this the current entry?
       if(entry.offset == current_data_offset) {
         continue;
       }

       // Does this entry overlap with check_at?
       if(entry.offset == check_at) {
         // This entry is at the same offset
         problem = 1;
         break;
       } else if(entry.offset < check_at && entry.offset + entry.size >= check_at) {
         // Overlap
         problem = 1;
         break;
       } else if(entry.offset > check_at && entry.offset + entry.size <= check_at) {
         // Overlap
         problem = 1;
         break;
       }
    }

    if(!problem) {
      // Offset does not overlap

      // Move data
      if(index->size != 0) {
        uint8_t *data = malloc(index->size); // TODO Move in pieces
        if(data == NULL) {
          WARNING();
          return 0;
        }
        if(!StorageRead(root, index->offset, index->size, data)) {
          WARNING();
          free(data);
          return 0;
        }
        if(!StorageWrite(root, check_at, index->size, data)) {
          WARNING();
          free(data);
          return 0;
        }
        free(data);
      }

      // Update index
      index->size = new_size;
      index->offset = check_at;

      // Update index in FS
      if(!Fs_HypnoticFS_UpdateIndex(root, index_data_offset, index)) {
        WARNING();
        return 0;
      } else {
        return 1;
      }
    }
  }

  // Find new offset
  if((new_data_offset = Fs_HypnoticFS_FindSpace(root, new_size)) == 0) {
    WARNING();
    return 0;
  }

  // Move data
  if(index->size != 0) {
    uint8_t *data = malloc(index->size); // TODO Move in pieces
    if(data == NULL) {
      WARNING();
      return 0;
    }
    if(!StorageRead(root, index->offset, index->size, data)) {
      WARNING();
      free(data);
      return 0;
    }
    if(!StorageWrite(root, new_data_offset, index->size, data)) {
      WARNING();
      free(data);
      return 0;
    }
    free(data);
  }

  // Update index
  index->size = new_size;
  index->offset = new_data_offset;

  // Update index in FS
  if(!Fs_HypnoticFS_UpdateIndex(root, index_data_offset, index)) {
    WARNING();
    return 0;
  } else {
    return 1;
  }
}

/**
 * Update an index entry in the FS index space.
 * @param  root              The FS root.
 * @param  index_data_offset The location of the index entry in the index space.
 * @param  index             The index entry data which will be written to the FS.
 * @return                   0 on error, 1 on success.
 */
uint8_t Fs_HypnoticFS_UpdateIndex(FsRoot_t *root, uint64_t index_data_offset, HypnoticFS_Index_t *index) {
  if(StorageWrite(root, index_data_offset, sizeof(HypnoticFS_Index_t), index) == 0) {
    WARNING();
    return 0;
  } else {
    return 1;
  }
}

/**
 * Generates an FsIndex_t struct using data from an HypnoticFS_Index_t struct.
 * @param  index The input.
 * @return       Return NULL on failure or the generated sturct on success.
 */
FsIndex_t *Fs_HypnoticFS_GenerateFsIndex(HypnoticFS_Index_t *index)
{
  FsIndex_t *to_ret;

  if((to_ret = malloc(sizeof(FsIndex_t))) == NULL)
    return NULL;

  to_ret->size = index->size;

  switch(index->type) {
  case HYPNOTICFS_TYPE_FILE:
    to_ret->type = INDEX_TYPE_FILE;
    break;

  case HYPNOTICFS_TYPE_DIRECTORY:
      to_ret->type = INDEX_TYPE_DIRECTORY;
      break;

  default:
    free(to_ret);
    return NULL;
  }

  return to_ret;
}
