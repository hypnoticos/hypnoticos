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

#include <stdio.h>
#include <stdlib.h>
#include <multiboot.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/boot.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/devices/storage/memory-disk.h>
#include <hypnoticos/fs.h>
#include <hypnoticos/fs/hypnoticfs.h>

#define INITIAL_SIZE 1024*1024
#define MODULE_FILE_NAME  "/module%u"

uint8_t BootLoadModules() {
  multiboot_module_t *module;
  uint32_t i, module_file_name_strlen;
  StorageDevice_MemoryDisk_t *storage_data;
  char *module_file_name, *initial_data;

  INFO(">> Initial memory disk");
  storage_data = malloc(sizeof(StorageDevice_MemoryDisk_t));
  storage_data->ptr = malloc(INITIAL_SIZE);
  storage_data->size = INITIAL_SIZE;

  StorageNew(DEVICE_TYPE_MEMORYDISK, storage_data);
  FsNewRoot(ROOT_TYPE_HYPNOTICFS, &(StorageDevices[0]), 0);

  initial_data = malloc(INDEX_START);
  snprintf(initial_data, MAGIC_SIZE + 1, "%c%s", MAGIC_1, MAGIC_STRING);
  uint64_t initial_index_size = INITIAL_SIZE * 0.05;
  memcpy((char *) (((uint64_t) initial_data) + MAGIC_SIZE), &initial_index_size, sizeof(uint64_t));
  StorageWrite(&(FsRoots[0]), 0, INDEX_START, initial_data);

  INFO(">> Modules");
  for(i = 0; i < BootModulesCount; i++) {
    module = (multiboot_module_t *) ((uint32_t) BootModulesAddr + (sizeof(multiboot_module_t) * i));

    // Add module as /moduleN
    module_file_name_strlen = snprintf(NULL, 0, MODULE_FILE_NAME, i);
    module_file_name = malloc(module_file_name_strlen + 1);
    snprintf(module_file_name, module_file_name_strlen, MODULE_FILE_NAME, i);
    INFO(">>>> %s", module_file_name);

    // Create file
    if(!FsNewIndex(module_file_name, INDEX_TYPE_FILE)) {
      WARNING();
      // TODO Clean up
      continue;
    }

    // Write to file
    if(FsWrite(module_file_name, 0, module->mod_end - module->mod_start, (uint8_t *) ((uint64_t) module->mod_start)) == 0) {
      WARNING();
      // TODO Clean up
      continue;
    }

    // Create process
    if(!DispatcherProcessNewFromFormat(module_file_name)) {
      WARNING();
      continue;
    }
  }

  return 1;
}
