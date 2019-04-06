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

void MemoryAllocated(void *addr, size_t size, const char function[200], uint32_t line) {
  MemoryTableIndex_t *mti;
  MemoryTable_t *mt;

  static uint8_t new_table_pending = 0;

  for(mti = &MemoryTableIndices; mti != NULL; mti = mti->next) {
    for(mt = mti->addr; (uint32_t) mt < (uint32_t) mti->addr + mti->size; mt = (MemoryTable_t *) ((uint32_t) mt + sizeof(MemoryTable_t))) {
      if(mt->status != 1) {
        // Not allocated
        mt->addr = (uint32_t) addr;
        mt->size = size;
        mt->status = 1;
        strcpy(mt->function, function);
        mt->line = line;
        return;
      }

      // If the iteration covers one of the last 5 entries in the final table, then create a new table
      // The process of creating a new table requires the use of malloc()
      if(mti->next == NULL && (uint32_t) mt >= (uint32_t) mti->addr + mti->size - (sizeof(MemoryTable_t) * 5) && new_table_pending == 0) {
        new_table_pending = 1;
        MemoryNewTable();
        new_table_pending = 0;
      }
    }
  }

  // TODO Replace with an error?
  HALT();
}

void *__malloc_align(size_t size, uint8_t align, const char function[200], uint32_t line) {
  void *addr;

  if(MemoryTableIndices.size == 0) {
    // No memory tables
    HALT();
  }

  // Find space
  if((addr = MemoryFindSpace(size, align)) == NULL) {
    printf("malloc: couldn't allocate\n");
    return NULL;
  }

  // Update table
  MemoryAllocated(addr, size, function, line);

  return addr;
}
