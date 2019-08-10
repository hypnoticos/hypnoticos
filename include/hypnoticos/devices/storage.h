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

#ifndef HYPNOTICOS_DEVICES_STORAGE_H
#define HYPNOTICOS_DEVICES_STORAGE_H

#include <stdint.h>

typedef struct _FsRoot_t FsRoot_t;

#define DEVICE_TYPE_END                 0x0
#define DEVICE_TYPE_RAMDISK             0x1

typedef struct _StorageDevice_t StorageDevice_t;
struct _StorageDevice_t {
  uint32_t device_type;
  void *device_type_data;
};

extern StorageDevice_t *StorageDevices;

uint8_t StorageInit();
void StorageNew(uint32_t device_type, void *device_type_data);
uint8_t StorageRead(FsRoot_t *root, uint64_t offset, uint64_t size, void *dest);
uint8_t StorageWrite(FsRoot_t *root, uint64_t offset, uint64_t size, void *source);

#endif
