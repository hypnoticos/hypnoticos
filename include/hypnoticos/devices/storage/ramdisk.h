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

#ifndef HYPNOTICOS_DEVICES_STORAGE_RAMDISK_H
#define HYPNOTICOS_DEVICES_STORAGE_RAMDISK_H

#include <stdint.h>
#include <hypnoticos/devices/storage.h>

typedef struct _StorageDevice_RamDisk_t StorageDevice_RamDisk_t;
struct _StorageDevice_RamDisk_t {
  void *ptr;
  uint64_t size;
};

uint8_t StorageRead_RamDisk(FsRoot_t *root, uint64_t offset, uint64_t size, void *dest);
uint8_t StorageWrite_RamDisk(FsRoot_t *root, uint64_t offset, uint64_t size, void *source);

#endif
