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

#ifndef HYPNOTICFS_H
#define HYPNOTICFS_H

#include <stdio.h>
#include <stdint.h>

#define ERROR(message)    fprintf(stderr, "Error: %s\n", message);

#define MAGIC_1       0xFF
#define MAGIC_STRING  "HypnoticFS"
#define MAGIC_SIZE    (1 + strlen(MAGIC_STRING))

#define INDEX_START   (MAGIC_SIZE + sizeof(uint64_t))

#define HYPNOTICFS_TYPE_EMPTY     0x00
#define HYPNOTICFS_TYPE_FILE      0x01
#define HYPNOTICFS_TYPE_DIRECTORY 0x02

#define HYPNOTICFS_FILENAME_SIZE  32
#define HYPNOTICFS_MAX_PATH_SIZE  1000

typedef struct _HypnoticFS_Index HypnoticFS_Index_t;
struct _HypnoticFS_Index {
  uint8_t type;
  uint64_t parent_offset;
  uint64_t size;
  uint64_t offset;
  char name[HYPNOTICFS_FILENAME_SIZE];
} __attribute__((packed));

uint8_t CheckOverlap(FILE *f, uint64_t index_size, uint64_t potential_offset, uint64_t file_size);
char **PathBreakdown(char *path);

#endif
