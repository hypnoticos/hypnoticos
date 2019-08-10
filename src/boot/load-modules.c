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

#include <multiboot.h>
#include <hypnoticos/boot.h>
#include <hypnoticos/devices/storage.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/fs/hypnoticfs.h>
#include <hypnoticos/devices/storage/ramdisk.h>
#include <string.h>
#include <stdlib.h>

uint8_t BootLoadModules() {
  multiboot_module_t *module;

  if(BootModulesCount != 1) {
    INFO("Support for only one module");
    HALT();
  }

  module = (multiboot_module_t *) ((uint64_t) BootModulesAddr);
  INFO("Attempting to load module which starts at 0x%X (%u bytes)", module->mod_start, module->mod_end - module->mod_start);

  // Check the magic number (TODO Move this to the init code for HypnoticFS)
  if(*((uint8_t *) ((uint64_t) module->mod_start)) != MAGIC_1) {
    return 0;
  } else if(memcmp((uint8_t *) ((uint64_t) module->mod_start + 1), MAGIC_STRING, MAGIC_SIZE - 1) != 0) {
    return 0;
  }

  INFO("Setting up RAM disk");
  StorageDevice_RamDisk_t *data = malloc(sizeof(StorageDevice_RamDisk_t));
  uint64_t ramdisk_size = module->mod_end - module->mod_start;
  data->ptr = malloc(ramdisk_size);
  data->size = ramdisk_size;
  INFO("data->ptr = 0x%p mod_start=0x%X ramdisk_size=%u", data->ptr, module->mod_start, ramdisk_size);
  memcpy(data->ptr, (void *) ((uint64_t) module->mod_start), ramdisk_size);

  StorageNew(DEVICE_TYPE_RAMDISK, data);

  // TODO To implement
  FsRoots->storage = &(StorageDevices[0]);
  FsRoots->storage_offset = 0;

  INFO("Done");

  return 1;
}
