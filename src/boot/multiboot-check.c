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

void MultibootCheck(uint32_t magic, multiboot_info_t *multiboot) {
  uint32_t offset, mmap_addr, mmap_length, modules_count, modules_addr;
  multiboot_memory_map_t *mmap_entry;

  // At a minimum the magic number should be right, and the mmap should be present.
  // TODO Modules and cmdline.

  // Check magic number
  if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    printf("magic != MULTIBOOT_BOOTLOADER_MAGIC\n");
    HALT();
  }

  // Check if module information is present
  // (Bit 3 of flags)
  if(!(multiboot->flags & 0x08)) {
    printf("No module information\n");
    modules_count = 0;
    modules_addr = 0;
  } else {
    modules_count = multiboot->mods_count;
    modules_addr = multiboot->mods_addr;
  }

  // Get mmap buffer
  // Bit 6 of flags must be set
  if(!(multiboot->flags & 0x40)) {
    printf("Couldn't retrieve mmap_length and mmap_addr\n");
    HALT();
  }

  // WARNING: From here the existence of the multiboot structure and its components (except mmap components for the rest of this function) is not guaranteed.
  mmap_addr = multiboot->mmap_addr;
  mmap_length = multiboot->mmap_length;

  // Parse each mmap entry in the buffer
  offset = 0;
  while(offset < mmap_length) {
    mmap_entry = (multiboot_memory_map_t *) ((uint32_t) mmap_addr + offset);

    // Check if the size parameter is invalid
    if(offset + mmap_entry->size > mmap_length || mmap_entry->size < 20) {
      HALT();
    }

    // Check for a reference to 64-bit address space
    if(mmap_entry->addr_high != 0) {
      // Starts in 64-bit address space. Not supported.
      printf("WARNING: Some memory ignored as it resides above 32-bit address space.\n");
    } else if(mmap_entry->len_high != 0) {
      // Spans into 64-bit address space. Stop at 0xFFFFFFFF.
      printf("WARNING: Some memory ignored as it spans above 32-bit address space.\n");
    } else {
      // Check the type
      switch(mmap_entry->type) {
        case MULTIBOOT_MEMORY_AVAILABLE:
        MemoryNewBlock(mmap_addr, mmap_length, modules_count, modules_addr, mmap_entry->addr_low, mmap_entry->len_low, MEMORYBLOCK_TYPE_AVAILABLE);
        break;

        case MULTIBOOT_MEMORY_RESERVED:
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: // TODO
        case MULTIBOOT_MEMORY_NVS: // TODO Must be preserved on hibernation
        case MULTIBOOT_MEMORY_BADRAM:
        break;

        default:
        printf("Unknown type.\n");
        HALT();
        break;
      }
    }

    offset += mmap_entry->size + sizeof(mmap_entry->size);
  }

  if(offset != mmap_length) {
    // Something didn't go right.
    HALT();
  }
}
