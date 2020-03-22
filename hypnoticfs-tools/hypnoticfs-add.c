//
// HypnoticFS Tools
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "hypnoticfs.h"

#define BUFFER_SIZE       1024

int main(int argc, char *argv[]) {
  char *image_file, *dest, *file_to_add, **dest_breakdown;
  unsigned char magic[MAGIC_SIZE], buffer[BUFFER_SIZE];
  uint64_t entry_offset, index_size, image_size, file_size, potential_offset, i, read_size, parent_offset, filename_index, new_entry_offset;
  FILE *f_image, *f_to_add;
  HypnoticFS_Index_t entry;
  uint8_t found_entry, found_parent, found_offset;

  if(argc != 4) {
    fprintf(stdout, "HypnoticOS  Copyright (C) 2019 jk30\n");
    fprintf(stdout, "HypnoticFS, tool to add files to an image v0.1\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "This tool will add a file to an HypnoticFS image.\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Usage: %s image dest to-add\n", argv[0]);
    fprintf(stdout, "  image       Image file\n");
    fprintf(stdout, "  dest        Destination file name\n");
    fprintf(stdout, "  to-add      File to add\n");
    return 1;
  }

  image_file = argv[1];
  dest = argv[2];
  file_to_add = argv[3];

  if(strlen(file_to_add) > HYPNOTICFS_FILENAME_SIZE) {
    ERROR("File name is too long.\n");
    return 1;
  }

  f_image = fopen(image_file, "r+b");
  if(f_image == NULL) {
    ERROR("Couldn't open image file.");
    return 1;
  }

  f_to_add = fopen(file_to_add, "rb");
  if(f_to_add == NULL) {
    ERROR("Couldn't open source file.");
    return 1;
  }

  // Get image file size
  fseek(f_image, 0, SEEK_END);
  image_size = ftell(f_image);
  fseek(f_image, 0, SEEK_SET);

  // Read the header
  if(fread(magic, MAGIC_SIZE, 1, f_image) != 1) {
    ERROR("Couldn't read magic number.");
    return 1;
  } else if(magic[0] != MAGIC_1 || memcmp(&(magic[1]), MAGIC_STRING, MAGIC_SIZE - 1) != 0) {
    ERROR("Magic number didn't match.");
    return 1;
  }
  if(fread(&index_size, sizeof(uint64_t), 1, f_image) != 1) {
    ERROR("Couldn't read index size.");
    return 1;
  } else if(index_size > image_size) {
    ERROR("Index size is larger than the image size.");
    return 1;
  }

  // Find the parent's offset
  dest_breakdown = PathBreakdown(dest);
  if(dest_breakdown == NULL) {
    ERROR("Internal error.");
    return 1;
  }
  if(dest_breakdown[0] == NULL) {
    ERROR("Internal error.");
    return 1;
  }
  parent_offset = 0;
  for(i = 0; dest_breakdown[i + 1] != NULL; i++) {
    found_parent = 0;
    if(strlen(dest_breakdown[i]) > HYPNOTICFS_FILENAME_SIZE) {
      ERROR("File name is too long.");
      return 1;
    }
    for(entry_offset = INDEX_START;
     entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
     entry_offset += sizeof(HypnoticFS_Index_t)) {
      fseek(f_image, entry_offset, SEEK_SET);
      if(fread(&entry, sizeof(HypnoticFS_Index_t), 1, f_image) != 1) {
        ERROR("Error reading an entry.");
        return 1;
      }

      if(entry.type != HYPNOTICFS_TYPE_DIRECTORY || entry.type == HYPNOTICFS_TYPE_EMPTY) {
        continue;
      }

      if(memcmp(entry.name, dest_breakdown[i], HYPNOTICFS_FILENAME_SIZE) == 0 && entry.parent_offset == parent_offset) {
        parent_offset = entry_offset;
        found_parent = 1;
        break;
      }
    }
    if(!found_parent) {
      ERROR("Directory not found.");
      return 1;
    }
  }

  // Check for files that have this name and the same parent offset
  for(filename_index = 0; dest_breakdown[filename_index + 1] != NULL; filename_index++);
  if(strlen(dest_breakdown[filename_index]) > HYPNOTICFS_FILENAME_SIZE) {
    ERROR("Directory name too long.");
    return 1;
  }
  for(entry_offset = INDEX_START;
   entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
   entry_offset += sizeof(HypnoticFS_Index_t)) {
    fseek(f_image, entry_offset, SEEK_SET);
    if(fread(&entry, sizeof(HypnoticFS_Index_t), 1, f_image) != 1) {
      ERROR("Couldn't read an entry.");
      return 1;
    }

    if(entry.type != HYPNOTICFS_TYPE_EMPTY) {
      if(entry.parent_offset == parent_offset && memcmp(entry.name, dest_breakdown[filename_index], HYPNOTICFS_FILENAME_SIZE) == 0) {
        ERROR("Destination file exists.");
        return 1;
      }
    }
  }

  // Find an empty entry
  found_entry = 0;
  for(entry_offset = INDEX_START;
   entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
   entry_offset += sizeof(HypnoticFS_Index_t)) {
    fseek(f_image, entry_offset, SEEK_SET);
    if(fread(&entry, sizeof(HypnoticFS_Index_t), 1, f_image) != 1) {
      ERROR("Couldn't read an entry.");
      return 1;
    }
    if(entry.type == HYPNOTICFS_TYPE_EMPTY) {
      found_entry = 1;
      break;
    }
  }
  if(!found_entry) {
    ERROR("No remaining indicies.");
    return 1;
  }
  new_entry_offset = entry_offset;

  // Get the file size
  fseek(f_to_add, 0, SEEK_END);
  file_size = ftell(f_to_add);
  fseek(f_to_add, 0, SEEK_SET);

  // Check file size against index size
  if(file_size >= image_size - index_size) {
    ERROR("Insufficient space.");
    return 1;
  }

  // Find an offset to add the data
  potential_offset = index_size;
  if(CheckOverlap(f_image, index_size, potential_offset, file_size) != 0) {
    found_offset = 0;
    for(entry_offset = INDEX_START;
     entry_offset + sizeof(HypnoticFS_Index_t) < index_size;
     entry_offset += sizeof(HypnoticFS_Index_t)) {
      fseek(f_image, entry_offset, SEEK_SET);
      if(fread(&entry, sizeof(HypnoticFS_Index_t), 1, f_image) != 1) {
        ERROR("Couldn't read an entry.");
        return 1;
      }
      if(entry.type == HYPNOTICFS_TYPE_EMPTY) {
        continue;
      }
      potential_offset = entry.offset + entry.size + 1;

      if(potential_offset < index_size) {
        continue;
      }

      if(CheckOverlap(f_image, index_size, potential_offset, file_size) == 0) {
        found_offset = 1;
        break;
      }
    }
    if(!found_offset) {
      ERROR("Insufficient remaining space in the image for this file.");
      return 1;
    }
  }

  // Add the file at the offset
  fseek(f_image, potential_offset, SEEK_SET);
  for(i = 0; i < file_size; i += 1024) {
    if(file_size - i < BUFFER_SIZE) {
      read_size = file_size - i;
    } else {
      read_size = BUFFER_SIZE;
    }
    if(fread(buffer, read_size, 1, f_to_add) != 1) {
      ERROR("Couldn't read the source file.");
      return 1;
    }
    if(fwrite(buffer, read_size, 1, f_image) != 1) {
      ERROR("Couldn't write to the image file.");
      return 1;
    }
  }

  memset(&entry, 0, sizeof(HypnoticFS_Index_t));
  memcpy(entry.name, dest_breakdown[filename_index], strlen(dest_breakdown[filename_index]));
  entry.parent_offset = parent_offset;
  entry.size = file_size;
  entry.type = HYPNOTICFS_TYPE_FILE;
  entry.offset = potential_offset;

  fseek(f_image, new_entry_offset, SEEK_SET);
  if(fwrite(&entry, sizeof(HypnoticFS_Index_t), 1, f_image) != 1) {
    ERROR("Couldn't write entry.");
    return 1;
  }

  fclose(f_image);
  fclose(f_to_add);

  // TODO Clean up

  return 0;
}
