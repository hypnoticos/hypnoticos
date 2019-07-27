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
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MemoryBlock_t MemoryBlocks = {.start=0, .length=0, .type=0, .prev=NULL, .next=NULL};
MemoryTableIndex_t MemoryTableIndices = {.addr=NULL, .size=NULL, .prev=NULL, .next=NULL};

inline uint8_t MemoryNewBlock_CheckOverlap(void *addr, uint64_t size, void *check_addr, void *check_addr_end);
inline uint8_t MemoryNewBlock_NewEntry(MemoryTable_t *mt_start, uint64_t mt_length, MemoryTable_t **last_mt, void *alloc_addr, uint64_t alloc_size);

/*!
   \brief Checks if check_addr to check_addr_end will overlap with addr/size.
   \param addr Start address
   \param size Size
   \param check_addr Start address
   \param check_addr_end End address
   \return 1 if it doesn't overlap, 0 if it does.
*/
inline uint8_t MemoryNewBlock_CheckOverlap(void *addr, uint64_t size, void *check_addr, void *check_addr_end) {
  if((uint64_t) check_addr_end < (uint64_t) addr) {
    return 1;
  } else if((uint64_t) check_addr > (uint64_t) addr + size) {
    return 1;
  } else {
    return 0;
  }
}

inline uint8_t MemoryNewBlock_NewEntry(MemoryTable_t *mt_start, uint64_t mt_length, MemoryTable_t **last_mt, void *alloc_addr, uint64_t alloc_size) {
  if(*last_mt == NULL) {
    *last_mt = mt_start;
  } else {
    if((uint64_t) (*last_mt) + sizeof(MemoryTable_t) >= (uint64_t) mt_start + mt_length) {
      return 0;
    }

    *last_mt = (MemoryTable_t *) (((uint64_t) last_mt) + sizeof(MemoryTable_t));
  }

  (*last_mt)->addr = (uint64_t) alloc_addr;
  (*last_mt)->size = alloc_size;
  strcpy((*last_mt)->function, "-");
  (*last_mt)->line = 0;
  (*last_mt)->status = 1;

  return 1;
}

#define MEMORY_FLOOR                    (0x100000)
#define MEMORY_TABLE_SKIP               (0x1000)

/*!
   \brief Create an entry for this memory block
   \param multiboot The address of the multiboot_info_t structure.
   \param mmap_entry The address of the multiboot_memory_map_t structure for this block.
   \return 1 on success, 0 on failure.
*/
// NOTE: Do not use INFO, WARNING or HALT here. The macros call printf which in turn calls malloc.
uint8_t MemoryNewBlock(multiboot_info_t *multiboot, multiboot_memory_map_t *mmap_entry) {
  uint8_t available, mt_exists, found_space;
  uint64_t start, length, mt_length, i;
  MemoryBlock_t *current, *next;
  void *mt_start;

  if(mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
    available = 1;
  } else if(mmap_entry->type == MULTIBOOT_MEMORY_RESERVED || mmap_entry->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE || mmap_entry->type == MULTIBOOT_MEMORY_NVS || mmap_entry->type == MULTIBOOT_MEMORY_BADRAM) {
    available = 0;
  } else {
    return 0;
  }

  // Check if there is already a memory table
  if(MemoryTableIndices.addr == NULL && MemoryTableIndices.size == NULL && MemoryTableIndices.next == NULL) {
    mt_exists = 0;
  } else {
    mt_exists = 1;
  }

  // Search for the last memory block
  for(current = &MemoryBlocks; current->next != NULL; current = current->next);

  // Do not allocate below MEMORY_FLOOR
  start = mmap_entry->addr;
  length = mmap_entry->len;
  if(start <= MEMORY_FLOOR) {
    if(start + length <= MEMORY_FLOOR) {
      // Do not allocate
      return 1;
    } else {
      start = MEMORY_FLOOR;
      length -= MEMORY_FLOOR;
    }
  }

  // Are there no memory blocks?
  if(current == &MemoryBlocks && MemoryBlocks.type == 0) {
    // Add the memory block to the first entry
    if(!available) {
      // This is the first block, and it is not available so we can't create a memory table on the block
      puts("Memory Error 1");
      return 0;
    }

    current->start = start;
    current->length = length;
    current->type = available;
  } else {
    // Is there a memory table?
    if(!mt_exists) {
      // Won't be able to allocate memory
      puts("Memory Error 2");
      return 0;
    }

    // Allocate memory for a new memory block struct
    next = malloc(sizeof(MemoryBlock_t)); // TODO Create a boolean which prevents the creation of another memory table until memory has been fully initialised

    current->next = next;

    next->start = start;
    next->length = length;
    next->type = available;
    next->prev = current;
    next->next = NULL;
  }

  // Is there a memory table?
  if(!mt_exists) {
    // Create a memory table

    if(!available) {
      // This block is not available so a memory table can't be created on it
      puts("Memory Error 3");
      return 0;
    }

    // Find a location which does not overwrite the multiboot structure
    mt_length = sizeof(MemoryTable_t) * MEMORY_TABLE_INITIAL_ENTRIES;
    found_space = 0;
    for(mt_start = (void *) start;
      (uint64_t) mt_start + mt_length < (uint64_t) start + length;
      mt_start = (void *) ((uint64_t) mt_start + MEMORY_TABLE_SKIP)) {
        // Check if this overlaps with the kernel
        if(!MemoryNewBlock_CheckOverlap(mt_start, mt_length, &AddrStart, &AddrEnd)) {
          continue;
        }

        // Check if this overlaps with any of the modules
        uint8_t overlaps = 0;
        for(i = 0; i < multiboot->mods_count; i++) {
          multiboot_module_t *module = (multiboot_module_t *) ((uint64_t) multiboot->mods_addr + (sizeof(multiboot_module_t) * i));
          if(!MemoryNewBlock_CheckOverlap(mt_start, mt_length, (void *) ((uint64_t) module->mod_start), (void *) ((uint64_t) module->mod_end))) {
            overlaps = 1;
            break;
          }
        }
        if(overlaps) {
          continue;
        }

        // Check if this overlaps with the multiboot structure
        if(!MemoryNewBlock_CheckOverlap(mt_start, mt_length, multiboot, (void *) ((uint64_t) multiboot + sizeof(multiboot)))) {
          continue;
        }

        // Check if this overlaps with any of the module entries
        if(!MemoryNewBlock_CheckOverlap(mt_start, mt_length, (void *) ((uint64_t) multiboot->mods_addr), (void *) ((uint64_t) multiboot->mods_addr + (multiboot->mods_count * sizeof(multiboot))))) {
          continue;
        }

        // Check if this overlaps with any of the mmap entries
        if(!MemoryNewBlock_CheckOverlap(mt_start, mt_length, (void *) ((uint64_t) multiboot->mmap_addr), (void *) ((uint64_t) multiboot->mmap_addr + multiboot->mmap_length))) {
          continue;
        }

        // Found space
        found_space = 1;
        break;
    }
    if(!found_space) {
      puts("Memory Error 4");
      return 0;
    }

    // Clear the area
    memset(mt_start, 0, mt_length);

    // Mark entries as allocated
    MemoryTable_t *last_mt = NULL;

    // The memory table itself
    if(!MemoryNewBlock_NewEntry(mt_start, mt_length, &last_mt, mt_start, mt_length)) {
      puts("Memory Error 5");
      return 0;
    }

    // The kernel
    if(!MemoryNewBlock_NewEntry(mt_start, mt_length, &last_mt, &AddrStart, (uint64_t) &AddrEnd - (uint64_t) &AddrStart)) {
      puts("Memory Error 6");
      return 0;
    }

    // The modules
    for(i = 0; i < multiboot->mods_count; i++) {
      multiboot_module_t *module = (multiboot_module_t *) ((uint64_t) multiboot->mods_addr + (sizeof(multiboot_module_t) * i));
      if(!MemoryNewBlock_NewEntry(mt_start, mt_length, &last_mt, (void *) ((uint64_t) module->mod_start), module->mod_end - module->mod_start)) {
        puts("Memory Error 7");
        return 0;
      }
    }

    // The multiboot structure
    if(!MemoryNewBlock_NewEntry(mt_start, mt_length, &last_mt, multiboot, sizeof(multiboot))) {
      puts("Memory Error 8");
      return 0;
    }

    // The module entries
    if(!MemoryNewBlock_NewEntry(mt_start, mt_length, &last_mt, (void *) ((uint64_t) multiboot->mods_addr), multiboot->mods_count * sizeof(multiboot))) {
      puts("Memory Error 9");
      return 0;
    }

    // The mmap entries
    if(!MemoryNewBlock_NewEntry(mt_start, mt_length, &last_mt, (void *) ((uint64_t) multiboot->mmap_addr), multiboot->mmap_length)) {
      puts("Memory Error 10");
      return 0;
    }

    // Create an entry in the memory table index
    MemoryTableIndices.addr = (MemoryTable_t *) mt_start;
    MemoryTableIndices.size = mt_length;
    MemoryTableIndices.next = NULL;
    MemoryTableIndices.prev = NULL;
  }

  return 1;
}
