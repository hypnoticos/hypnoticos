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

#include <stdlib.h>
#include <string.h>
#include <multiboot.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/boot.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/devices/storage/memory-disk.h>
#include <hypnoticos/fs.h>

uint8_t BootLoadModules() {
  multiboot_module_t *module;
  uint64_t module_size;
  StorageDevice_MemoryDisk_t *storage_data;

  if(BootModulesCount != 1) {
    WARNING();
    return 0;
  }

  // Check first module
  module = (multiboot_module_t *) ((uint64_t) BootModulesAddr);
  if(module->mod_end - module->mod_start == 0) {
    WARNING();
    return 0;
  }

  // Copy module
  module_size = module->mod_end - module->mod_start;
  storage_data = malloc(sizeof(StorageDevice_MemoryDisk_t));
  storage_data->ptr = malloc(module_size);
  memcpy(storage_data->ptr, (void *) ((uint64_t) module->mod_start), module_size);
  storage_data->size = module_size;

  StorageNew(DEVICE_TYPE_MEMORYDISK, storage_data);
  FsNewRoot(ROOT_TYPE_HYPNOTICFS, &(StorageDevices[0]), 0);

  // Create process
  if(!DispatcherProcessNewFromFormat("/bin/console", NULL, 0)) {
    WARNING();
    return 0;
  } else {
    return 1;
  }
}
