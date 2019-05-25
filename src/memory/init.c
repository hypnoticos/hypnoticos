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

#define NEXT_ENTRY()                          table = (MemoryTable_t *) (((uint32_t) table) + sizeof(MemoryTable_t)); if((uint32_t) table + sizeof(MemoryTable_t) >= start + table_size) { HALT(); }

/*!< A linked list containing the known available memory blocks */
MemoryBlock_t MemoryBlocks = {.start=0, .length=0, .type=0, .prev=NULL, .next=NULL};

MemoryTableIndex_t MemoryTableIndices = {.addr=NULL, .size=NULL, .prev=NULL, .next=NULL};

void MemoryNewBlock(uint32_t mmap_addr, uint32_t mmap_length, uint32_t start, uint32_t length, uint8_t type) {
  MemoryBlock_t *current, *next;
  uint32_t table_size, i;
  MemoryTable_t *table;
  multiboot_module_t *module;

  if(start < 0x100000) {
    // Ignore < 1MB
    return;
  }

  for(current = &MemoryBlocks; current->next != NULL; current = current->next);

  if(current == &MemoryBlocks && MemoryBlocks.type == 0) {
    // There are no other entries in the list
    current->start = start;
    current->length = length;
    current->type = type;
  } else {
    // Create a new entry
    next = malloc(sizeof(MemoryBlock_t)); // TODO What if this (or something that this function does - like creating a new table) overwrites mmap_addr or mmap_length?
    current->next = next;

    next->start = (start == 0 ? start + 1 : start);
    next->length = (start == 0 ? length - 1 : length);
    next->type = type;
    next->prev = current;
    next->next = NULL;
  }

#if defined(MT_START) || defined(MT_END) || defined(MMAP_START) || defined(MMAP_END)
#error Macro conflict
#endif
#define MT_START        (void *) start
#define MT_END          (void *) (start + table_size)
#define MMAP_START      (void *) mmap_addr
#define MMAP_END        (void *) (mmap_addr + mmap_length)

  // Check if there is a memory table
  if(MemoryTableIndices.size == 0) {
    table_size = sizeof(MemoryTable_t) * MEMORY_TABLE_INITIAL_ENTRIES;

    // Is the block big enough to create a table?
    if(length < table_size) {
      HALT();
    }

    // Create a memory table
    // First check if creating it at the start of this block would create a problem
    if(!((void **) MT_START < &AddrStart && (void **) MT_END < &AddrStart)) {
      // TODO Place the memory table elsewhere
      // May overwrite the kernel
      HALT();
    } else if(!(MT_START < MMAP_START && MT_END < MMAP_START)) {
      // TODO Place the memory table elsewhere
      // May overwrite mmap entries (which are currently being processed)
      HALT();
    }

    // Check if the memory table would overlap with module information or the modules themselves
    if(BootModulesCount != 0 && BootModulesAddr != 0) {
      if(!((uint32_t) MT_START < BootModulesAddr && (uint32_t) MT_END < BootModulesAddr)) {
        // TODO Place the memory table elsewhere
        // May overwrite module information entries
        HALT();
      }

      for(i = 0; i < BootModulesCount; i++) {
        module = (multiboot_module_t *) ((uint32_t) BootModulesAddr + (sizeof(multiboot_module_t) * i));
        if(!((uint32_t) MT_START < module->mod_start && (uint32_t) MT_END < module->mod_start)) {
          // TODO Place the memory table elsewhere
          // May overwrite this module
          HALT();
        }
      }
    }

    // Clear the area
    memset((void *) start, 0, table_size);

#if MEMORY_TABLE_INITIAL_ENTRIES < 2
#error Initial memory table is too small
#endif

    // Create an entry in the table for the table, for the kernel, the module information and the modules
    table = (MemoryTable_t *) start;
    table->addr = (uint32_t) start;
    table->size = table_size;
    strcpy(table->function, "-");
    table->line = 0;
    table->status = 1;

    NEXT_ENTRY();
    table->addr = (uint32_t) &AddrStart;
    table->size = (uint32_t) &AddrEnd - (uint32_t) &AddrStart + 1;
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
      module = (multiboot_module_t *) ((uint32_t) BootModulesAddr + (sizeof(multiboot_module_t) * i));

      NEXT_ENTRY();
      table->addr = module->mod_start;
      table->size = module->mod_end - module->mod_start;
      strcpy(table->function, "-");
      table->line = 0;
      table->status = 1;
    }

    // Create an entry in the memory table index
    MemoryTableIndices.addr = (MemoryTable_t *) start;
    MemoryTableIndices.size = table_size;
    MemoryTableIndices.next = NULL;
    MemoryTableIndices.prev = NULL;
  }
}
