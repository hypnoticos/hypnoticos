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
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>

uint8_t MemoryAllocated(void *addr, size_t size, const char *function, uint32_t line) {
  MemoryTableIndex_t *mti;
  MemoryTable_t *mt;

  static uint8_t new_table_pending = 0;

  for(mti = &MemoryTableIndices; mti != NULL; mti = mti->next) {
    for(mt = mti->addr; (uint64_t) mt < (uint64_t) mti->addr + mti->size; mt = (MemoryTable_t *) ((uint64_t) mt + sizeof(MemoryTable_t))) {
      if(mt->status != 1) {
        // Not allocated
        mt->addr = (uint64_t) addr;
        mt->size = size;
        mt->status = 1;
        strcpy(mt->function, function);
        mt->line = line;
        return 1;
      }

      // If the iteration covers one of the last 10 entries in the final table, then create a new table
      // The process of creating a new table requires the use of malloc()
      if(mti->next == NULL && (uint64_t) mt >= (uint64_t) mti->addr + mti->size - (sizeof(MemoryTable_t) * 10) && new_table_pending == 0) {
        new_table_pending = 1;
        MemoryNewTable();
        new_table_pending = 0;

        // Restart the search for an address - the address to be allocated may have now been taken partially or fully by MemoryNewTable();
        return 0;
      }
    }
  }

  // TODO Replace with an error?
  HALT();
}

void *__malloc_align(size_t size, uint8_t align, const char *function, uint32_t line) {
  void *addr;
  uint8_t restarted = 0;

restart:
  if(MemoryTableIndices.size == 0) {
    // No memory tables
    HALT();
  }

  // Find space
  if((addr = MemoryFindSpace(size, align)) == NULL) {
    INFO("couldn't allocate %u bytes (%s %u)\n", (uint32_t) size, function, line);
    return NULL;
  }

  // Update table
  if(!MemoryAllocated(addr, size, function, line)) {
    // To restart the search

    if(restarted) {
      HALT();
    }

    restarted = 1;
    goto restart;
  }

  return addr;
}
