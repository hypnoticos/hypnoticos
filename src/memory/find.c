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
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>

/*!
   \brief For an address, find the that address's MemoryTable_t structure.
   \param addr The address
   \return The structure, or NULL if the address isn't allocated.
*/
inline MemoryTable_t *MemoryFind(void *addr) {
  MemoryTableIndex_t *mti;
  MemoryTable_t *mt;

  for(mti = &MemoryTableIndices; mti != NULL; mti = mti->next) {
    for(mt = mti->addr; mt < mti->addr + mti->size; mt += sizeof(MemoryTable_t)) {
      if(mt->status != 1) {
        // Not allocated
        continue;
      }

      if(addr <= (void *) mt->addr || addr >= (void *) mt->addr + mt->size) {
        // The address is before or after this allocation
        continue;
      }

      // Otherwise, this is the allocation
      return mt;
    }
  }

  return NULL;
}

void *MemoryFindSpace(size_t size) {
  MemoryBlock_t *block;
  MemoryTable_t *mt;
  uint32_t addr = 0;

  // For each memory block, find the highest address possible
  for(block = &MemoryBlocks; block != NULL; block = block->next) {
    addr = block->start + block->length - 1 - size;

    // Check if this address is available
    // If not, check before the start of the address occupying that address. Continue until space found, or no space is available.
    do {
      if((mt = MemoryFind((void *) addr)) != NULL) {
        addr = mt->addr - size;
      } else {
        return (void *) addr;
      }
    } while(addr >= block->start);
  }

  return NULL;
}
