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
#include <string.h>
#include <stdlib.h>
#include <hypnoticos/boot.h>
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>
#include <multiboot.h>

#define MEMORY_TABLE_INITIAL_ENTRIES          100

#define MEMORY_ERROR(text)                    puts("Memory error"); puts(text) HALT_NO_OUTPUT();

#define NEXT_ENTRY()                          table = (MemoryTable_t *) (((uint64_t) table) + sizeof(MemoryTable_t)); if((uint64_t) table + sizeof(MemoryTable_t) >= mt_start + mt_size) { HALT(); }

/*!< A linked list containing the known available memory blocks */
MemoryBlock_t MemoryBlocks = {.start=0, .length=0, .type=0, .prev=NULL, .next=NULL};

MemoryTableIndex_t MemoryTableIndices = {.addr=NULL, .size=NULL, .prev=NULL, .next=NULL};

void MemoryNewBlock(uint32_t mmap_addr, uint32_t mmap_length, uint64_t start, uint64_t length, uint8_t type) {
  MemoryBlock_t *current, *next;
  uint64_t mt_start, mt_size, i;
  MemoryTable_t *table;
  multiboot_module_t *module;

  if(start < 0x100000) {
    // Ignore < 1MB
    return;
  }

  for(current = &MemoryBlocks; current->next != NULL; current = current->next);

  if(current == &MemoryBlocks && MemoryBlocks.type == 0) {
    // There are no other entries in the list
    if(type == MEMORYBLOCK_TYPE_UNAVAILABLE) {
      MEMORY_ERROR("Error 1");
    }

    current->start = start;
    current->length = length;
    current->type = type;
  } else {
    // Create a new entry
    next = malloc(sizeof(MemoryBlock_t)); // TODO What if this (or something that this function does - like creating a new table) overwrites mmap_addr or mmap_length?
    current->next = next;

    next->start = start;
    next->length = length;
    next->type = type;
    next->prev = current;
    next->next = NULL;
  }

#if defined(MMAP_START) || defined(MMAP_END)
#error Macro conflict
#endif
#define MMAP_START      (mmap_addr)
#define MMAP_END        (mmap_addr + mmap_length)

  // Check if there is a memory table
  if(MemoryTableIndices.size == 0) {
    mt_start = start;
    mt_size = sizeof(MemoryTable_t) * MEMORY_TABLE_INITIAL_ENTRIES;

    if(mt_start < (uint64_t) &AddrEnd) {
      mt_start = ((uint64_t) &AddrEnd) + 1;
      if(((uint64_t) &AddrEnd) + mt_size >= start + length) {
        MEMORY_ERROR("Error 2");
      }
    }

    // Is the block big enough to create a table?
    if(length < mt_size) {
      MEMORY_ERROR("Error 3");
    }

    // Create a memory table
    // First check if creating it here would create a problem
    if(((void **) mt_start < &AddrStart && (void **) (mt_start + mt_size) < &AddrStart)) {
      // TODO Place the memory table elsewhere
      // May overwrite the kernel
      MEMORY_ERROR("Error 4");
    } else if((mt_start < MMAP_START && (mt_start + mt_size) < MMAP_START)) {
      // TODO Place the memory table elsewhere
      // May overwrite mmap entries (which are currently being processed)
      MEMORY_ERROR("Error 5");
    }

    // Check if the memory table would overlap with module information or the modules themselves
    if(BootModulesCount != 0 && BootModulesAddr != 0) {
      if((mt_start < BootModulesAddr && mt_start + mt_size < BootModulesAddr)) {
        // TODO Place the memory table elsewhere
        // May overwrite module information entries
        MEMORY_ERROR("Error 6");
      }

      for(i = 0; i < BootModulesCount; i++) {
        module = (multiboot_module_t *) ((uint64_t) BootModulesAddr + (sizeof(multiboot_module_t) * i));
        if(mt_start > module->mod_end) {
          continue;
        } else if(mt_start + mt_size < module->mod_start) {
          continue;
        } else {
          // TODO Place the memory table elsewhere
          // May overwrite this module
          MEMORY_ERROR("Error 7");
        }
      }
    }

    // Clear the area
    memset((void *) mt_start, 0, mt_size);

#if MEMORY_TABLE_INITIAL_ENTRIES < 2
#error Initial memory table is too small
#endif

    // Create an entry in the table for the table, for the kernel, the module information and the modules
    table = (MemoryTable_t *) mt_start;
    table->addr = mt_start;
    table->size = mt_size;
    strcpy(table->function, "-");
    table->line = 0;
    table->status = 1;

    NEXT_ENTRY();
    table->addr = (uint64_t) &AddrStart;
    table->size = (uint64_t) &AddrEnd - (uint64_t) &AddrStart + 1;
    strcpy(table->function, "-");
    table->line = 0;
    table->status = 1;

    NEXT_ENTRY();
    table->addr = BootModulesAddr;
    table->size = BootModulesCount * sizeof(multiboot_module_t);
    strcpy(table->function, "-");
    table->line = 0;
    table->status = 1;

    for(i = 0; i < BootModulesCount; i++) {
      module = (multiboot_module_t *) ((uint64_t) BootModulesAddr + (sizeof(multiboot_module_t) * i));

      NEXT_ENTRY();
      table->addr = module->mod_start;
      table->size = module->mod_end - module->mod_start;
      strcpy(table->function, "-");
      table->line = 0;
      table->status = 1;
    }

    // Create an entry in the memory table index
    MemoryTableIndices.addr = (MemoryTable_t *) mt_start;
    MemoryTableIndices.size = mt_size;
    MemoryTableIndices.next = NULL;
    MemoryTableIndices.prev = NULL;
  }
}
