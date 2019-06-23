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

inline uint8_t MemoryAlign(uint64_t *addr, uint64_t min, uint64_t align);
uint8_t MemoryIsFree(void *addr, size_t size, void **end);

/*!
   \brief Find an address's MemoryTable_t structure.
   \param addr The address
   \return The structure, or NULL if the address wasn't found.
*/
inline MemoryTable_t *MemoryFind(void *addr) {
  MemoryTableIndex_t *mti;
  MemoryTable_t *mt;

  for(mti = &MemoryTableIndices; mti != NULL; mti = mti->next) {
    for(mt = mti->addr; (uint64_t) mt < (uint64_t) mti->addr + mti->size; mt = (MemoryTable_t *) ((uint64_t) mt + sizeof(MemoryTable_t))) {
      if(mt->status != 1) {
        // Not allocated
        continue;
      } else if((void *) mt->addr == addr) {
        // This is the allocation
        return mt;
      }
    }
  }

  WARNING();
  return NULL;
}

/*!
   \brief Check if the entire block specified is unallocated
   \param addr The start of the block
   \param size The size of the block
   \param end The end of the block
   \return 1 if unallocated, 0 otherwise.
*/
uint8_t MemoryIsFree(void *addr, size_t size, void **end) {
  MemoryTableIndex_t *mti;
  MemoryTable_t *mt;

  // Iterate through every memory allocation
  for(mti = &MemoryTableIndices; mti != NULL; mti = mti->next) {
    for(mt = mti->addr; (uint64_t) mt < (uint64_t) mti->addr + mti->size; mt = (MemoryTable_t *)((uint64_t) mt + sizeof(MemoryTable_t))) {
      if(mt->status != 1) {
        // Not allocated
        continue;
      }

      // new block's end is less than this block's start
      if((void *) ((uint64_t) addr + size) < (void *) mt->addr) {
        continue;
      }

      // new block's start is greater than this block's end
      if(addr >= (void *) ((uint64_t) mt->addr + mt->size)) {
        continue;
      }

      // Otherwise, this block overlaps with the specified block
      *end = (void *) ((uint64_t) mt->addr + mt->size);
      return 0;
    }
  }

  // No conflicts, entire block is unallocated
  return 1;
}

inline uint8_t MemoryAlign(uint64_t *addr, uint64_t min, uint64_t align) {
  uint64_t r;

  r = *addr;
  r = r % align;
  r = (*addr) - r;

  if(r < min) {
    WARNING();
    return 0;
  } else {
    *addr = r;
    return 1;
  }
}

void *MemoryFindSpace(size_t size, uint8_t align) {
  MemoryBlock_t *block;
  uint64_t addr = 0, max_addr;
  void *end;

  // Iterate through each memory block
  for(block = &MemoryBlocks; block != NULL; block = block->next) {
    if(block->type != MEMORYBLOCK_TYPE_AVAILABLE) {
      continue;
    }

    addr = block->start;
    max_addr = block->start + block->length;
    if(align == ALIGN_4KB) {
      if(!MemoryAlign(&addr, block->start, 4096)) {
        continue;
      }
    }

    // Check if this address is available
    // If not, check after the end of that address
    do {
      if(!MemoryIsFree((void *) addr, size, &end)) {
        if(align == ALIGN_4KB) {
          addr += 4096;
        } else {
          addr = (uint64_t) end;
        }
      } else {
        return (void *) addr;
      }
    } while(addr < max_addr);
  }

  WARNING();
  return NULL;
}
