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

uint8_t MemoryIsFree(void *addr, size_t size);

/*!
   \brief Find an address's MemoryTable_t structure.
   \param addr The address
   \return The structure, or NULL if the address wasn't found.
*/
inline MemoryTable_t *MemoryFind(void *addr) {
  MemoryTableIndex_t *mti;
  MemoryTable_t *mt;

  for(mti = &MemoryTableIndices; mti != NULL; mti = mti->next) {
    for(mt = mti->addr; (uint32_t) mt < (uint32_t) mti->addr + mti->size; mt = (MemoryTable_t *) ((uint32_t) mt + sizeof(MemoryTable_t))) {
      if(mt->status != 1) {
        // Not allocated
        continue;
      } else if((void *) mt->addr == addr) {
        // This is the allocation
        return mt;
      }
    }
  }

  return NULL;
}

/*!
   \brief Check if the entire block specified is unallocated
   \param addr The start of the block
   \param size The size of the block
   \return 1 if unallocated, 0 otherwise.
*/
uint8_t MemoryIsFree(void *addr, size_t size) {
  MemoryTableIndex_t *mti;
  MemoryTable_t *mt;

  // Iterate through every memory allocation
  for(mti = &MemoryTableIndices; mti != NULL; mti = mti->next) {
    for(mt = mti->addr; (uint32_t) mt < (uint32_t) mti->addr + mti->size; mt = (MemoryTable_t *)((uint32_t) mt + sizeof(MemoryTable_t))) {
      if(mt->status != 1) {
        // Not allocated
        continue;
      }

      // new block's end is less than this block's start
      if((void *) ((uint32_t) addr + size) < (void *) mt->addr) {
        continue;
      }

      // new block's start is greater than this block's end
      if(addr > (void *) ((uint32_t) mt->addr + mt->size)) {
        continue;
      }

      // Otherwise, this block overlaps with the specified block
      return 0;
    }
  }

  // No conflicts, entire block is unallocated
  return 1;
}

void *MemoryFindSpace(size_t size) {
  MemoryBlock_t *block;
  uint32_t addr = 0;

  // For each memory block, find the highest address possible
  for(block = &MemoryBlocks; block != NULL; block = block->next) {
    addr = block->start + block->length - 1 - size;

    // Check if this address is available
    // If not, check before the start of the address occupying that address. Continue until space found, or no space is available.
    do {
      if(!MemoryIsFree((void *) addr, size)) {
        addr -= size;
      } else {
        return (void *) addr;
      }
    } while(addr >= block->start);
  }

  return NULL;
}
