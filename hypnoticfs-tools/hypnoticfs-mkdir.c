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
  char *image_file, *directory, **dest_breakdown;
  unsigned char magic[MAGIC_SIZE];
  uint64_t entry_offset, index_size, image_size, i, parent_offset, name_index, new_entry_offset;
  FILE *f_image;
  HypnoticFS_Index_t entry;
  uint8_t found_entry, found_parent;

  if(argc != 3) {
    fprintf(stdout, "HypnoticOS  Copyright (C) 2019 jk30\n");
    fprintf(stdout, "HypnoticFS, tool to create directories on an image v0.1\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "This tool will create a directory on an HypnoticFS image.\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Usage: %s image directory\n", argv[0]);
    fprintf(stdout, "  image       Image file\n");
    fprintf(stdout, "  directory   Directory name\n");
    return 1;
  }

  image_file = argv[1];
  directory = argv[2];

  f_image = fopen(image_file, "r+b");
  if(f_image == NULL) {
    ERROR("Couldn't open image file.");
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
  dest_breakdown = PathBreakdown(directory);
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

  // Check for indices that have this name and the same parent offset
  for(name_index = 0; dest_breakdown[name_index + 1] != NULL; name_index++);
  if(strlen(dest_breakdown[name_index]) > HYPNOTICFS_FILENAME_SIZE) {
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
      if(entry.parent_offset == parent_offset && memcmp(entry.name, dest_breakdown[name_index], HYPNOTICFS_FILENAME_SIZE) == 0) {
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

  memset(&entry, 0, sizeof(HypnoticFS_Index_t));
  memcpy(entry.name, dest_breakdown[name_index], strlen(dest_breakdown[name_index]));
  entry.parent_offset = parent_offset;
  entry.size = 0;
  entry.type = HYPNOTICFS_TYPE_DIRECTORY;
  entry.offset = 0;

  fseek(f_image, new_entry_offset, SEEK_SET);
  if(fwrite(&entry, sizeof(HypnoticFS_Index_t), 1, f_image) != 1) {
    ERROR("Couldn't write entry.");
    return 1;
  }

  fclose(f_image);

  // TODO Clean up

  return 0;
}
