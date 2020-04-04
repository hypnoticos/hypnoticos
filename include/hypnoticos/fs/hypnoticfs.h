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

#ifndef HYPNOTICOS_FS_HYPNOTICFS_H
#define HYPNOTICOS_FS_HYPNOTICFS_H

#include <hypnoticos/fs.h>
#include <string.h>

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

uint64_t Fs_HypnoticFS_FindSpace(FsRoot_t *root, uint64_t size);
FsIndex_t *Fs_HypnoticFS_GenerateFsIndex(HypnoticFS_Index_t *index);
uint64_t Fs_HypnoticFS_GetEmptyIndex(FsRoot_t *root);
uint64_t Fs_HypnoticFS_GetIndexSize(FsRoot_t *root);
uint64_t Fs_HypnoticFS_GetIndex(FsRoot_t *root, char **path_short, HypnoticFS_Index_t *index);
uint64_t Fs_HypnoticFS_Resize(FsRoot_t *root, uint64_t index_data_offset, HypnoticFS_Index_t *index, uint64_t new_size);
uint8_t Fs_HypnoticFS_UpdateIndex(FsRoot_t *root, uint64_t index_data_offset, HypnoticFS_Index_t *index);

FsIndex_t *FsDetailsGet_HypnoticFS(FsRoot_t *root, const char *path, char **path_short);
FsIndex_t **FsList_HypnoticFS(FsRoot_t *root, const char *path, char **path_short);
uint8_t FsNewIndex_HypnoticFS(FsRoot_t *root, const char *path, char **path_short, uint8_t type);
uint64_t FsRead_HypnoticFS(FsRoot_t *root, const char *path, char **path_short, uint64_t offset, uint64_t size, uint8_t *dest);
uint64_t FsWrite_HypnoticFS(FsRoot_t *root, const char *path, char **path_short, uint64_t offset, uint64_t size, uint8_t *src);

#endif
