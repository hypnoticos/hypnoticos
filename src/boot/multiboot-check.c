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
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/boot.h>
#include <multiboot.h>
#include <hypnoticos/memory.h>

uint32_t BootModulesCount = 0;
uint32_t BootModulesAddr = 0;

void MultibootCheck(uint32_t magic, multiboot_info_t *multiboot) {
  uint32_t offset, mmap_addr, mmap_length;
  multiboot_memory_map_t *mmap_entry;

  // At a minimum the magic number should be right, and the mmap should be present.
  // TODO Modules and cmdline.

  // Check magic number
  if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    puts("magic != MULTIBOOT_BOOTLOADER_MAGIC");
    HALT_NO_OUTPUT();
  }

  // Check if module information is present
  // (Bit 3 of flags)
  if(!(multiboot->flags & 0x08)) {
    puts("No module information");
    HALT_NO_OUTPUT();
  }

  BootModulesCount = multiboot->mods_count;
  BootModulesAddr = multiboot->mods_addr;

  // Get mmap buffer
  // Bit 6 of flags must be set
  if(!(multiboot->flags & 0x40)) {
    puts("Couldn't retrieve mmap_length and mmap_addr");
    HALT_NO_OUTPUT();
  }

  // WARNING: From here the existence of the multiboot structure and its components (except mmap components for the rest of this function) is not guaranteed.
  mmap_addr = multiboot->mmap_addr;
  mmap_length = multiboot->mmap_length;

  // Parse each mmap entry in the buffer
  offset = 0;
  while(offset < mmap_length) {
    mmap_entry = (multiboot_memory_map_t *) ((uint64_t) mmap_addr + offset);

    // Check if the size parameter is invalid
    if(offset + mmap_entry->size > mmap_length || mmap_entry->size < 20) {
      puts("Error");
      HALT_NO_OUTPUT();
    }

    if(!MemoryNewBlock(multiboot, mmap_entry)) {
      puts("Error");
      HALT_NO_OUTPUT();
    }

    offset += mmap_entry->size + sizeof(mmap_entry->size);
  }

  if(offset != mmap_length) {
    // Something didn't go right.
    puts("Error");
    HALT_NO_OUTPUT();
  }
}
