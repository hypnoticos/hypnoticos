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
#include <hypnoticos/devices/storage/ramdisk.h>
#include <hypnoticos/hypnoticos.h>
#include <string.h>

uint8_t StorageWrite_RamDisk(FsRoot_t *root, uint64_t offset, uint64_t size, void *source) {
  StorageDevice_RamDisk_t *data;

  data = (StorageDevice_RamDisk_t *) root->storage->device_type_data;

  if(offset + size > data->size) {
    WARNING();
    return 0;
  }

  memcpy((void *) ((uint64_t) data->ptr + offset), source, size);

  return 1;
}
