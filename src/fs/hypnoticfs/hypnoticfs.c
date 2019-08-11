//
// HypnoticOS
// Copyright (C) 2019  jk30
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
#include <hypnoticos/fs/hypnoticfs.h>
#include <hypnoticos/hypnoticos.h>
#include <string.h>
#include <stdlib.h>

uint64_t Fs_HypnoticFS_GetIndexSize(FsRoot_t *root) {
  uint64_t index_size;

  if(!StorageRead(root, MAGIC_SIZE, sizeof(uint64_t), (void *) (&index_size))) {
    WARNING();
    return 0;
  } else {
    return index_size;
  }
}


HypnoticFS_Index_t *Fs_HypnoticFS_GetIndex(FsRoot_t *root, const char *path, char **path_short) {
  uint64_t i, parent_offset, index_size, filename_index;
  HypnoticFS_Index_t entry;
  HypnoticFS_Index_t *r; // TODO Caller must free this

  if(path_short[0] == NULL) {
    WARNING();
    return NULL;
  }

  if((index_size = Fs_HypnoticFS_GetIndexSize(root)) < INDEX_START) {
    WARNING();
    return NULL;
  }

  // Find the parent's offset
  parent_offset = 0;
  for(i = 0; path_short[i + 1] != NULL; i++) {
    uint8_t found_parent = 0;

    if(strlen(path_short[i]) > HYPNOTICFS_FILENAME_SIZE) {
      WARNING();
      return NULL;
    }

    for(uint64_t entry_offset = INDEX_START;
     entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
     entry_offset += sizeof(HypnoticFS_Index_t)) {
      if(!StorageRead(root, entry_offset, sizeof(HypnoticFS_Index_t), &entry)) {
        WARNING();
        return NULL;
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
      WARNING();
      return NULL;
    }
  }

  // Find files with the file name and the parent offset
  for(filename_index = 0; path_short[filename_index + 1] != NULL; filename_index++);
  if(strlen(path_short[filename_index]) > HYPNOTICFS_FILENAME_SIZE) {
    WARNING();
    return NULL;
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
      return NULL;
    }

    if(entry.type != HYPNOTICFS_TYPE_EMPTY) {
      if(entry.parent_offset == parent_offset && memcmp(entry.name, buffer, HYPNOTICFS_FILENAME_SIZE) == 0) {
        found_file = 1;
        break;
      }
    }
  }
  if(!found_file) {
    WARNING();
    return NULL;
  }

  // Prepare details about this file
  r = malloc(sizeof(HypnoticFS_Index_t));
  memcpy(r, &entry, sizeof(HypnoticFS_Index_t));

  // Return the details
  return r;
}
