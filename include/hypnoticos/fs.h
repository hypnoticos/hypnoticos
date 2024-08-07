//
// HypnoticOS
// Copyright (C) 2019, 2020, 2024  jk30
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

#ifndef HYPNOTICOS_FS_H
#define HYPNOTICOS_FS_H

#include <stdint.h>
#include <hypnoticos/devices/storage.h>

#define FS_MAX_PATH_SIZE                1000
#define FS_MAX_NAME_SIZE                100

#define ROOT_TYPE_END                   0x0
#define ROOT_TYPE_HYPNOTICFS            0x1

#define INDEX_TYPE_FILE                 0x1
#define INDEX_TYPE_DIRECTORY            0x2

typedef struct _FsRoot_t FsRoot_t;
struct _FsRoot_t {
  char *root;
  uint32_t fs_type;
  void *fs_type_data;

  StorageDevice_t *storage;
  uint64_t storage_offset;
};

typedef struct _FsIndex_t FsIndex_t;
struct _FsIndex_t {
  uint8_t type;
  uint64_t size;
  char name[FS_MAX_NAME_SIZE + 1];
};

extern FsRoot_t *FsRoots;

FsIndex_t *FsDetailsGet(const char *path);
uint8_t FsInit();
FsIndex_t **FsList(const char *path);
uint8_t FsNewIndex(const char *path, uint8_t type);
void FsNewRoot(uint32_t fs_type, StorageDevice_t *storage, uint64_t storage_offset);
uint64_t FsRead(const char *path, uint64_t offset, uint64_t size, uint8_t *dest);
uint64_t FsWrite(const char *path, uint64_t offset, uint64_t size, uint8_t *src);
char **PathBreakdown(const char *path);
uint8_t PathConsolidate(const char *path, char *dest);

#endif
