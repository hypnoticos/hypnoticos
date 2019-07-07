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

// NOTE: The malloc() implementation for the kernel is a macro
#ifndef _HYPNOTICOS_KERNEL

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/interface.h>
#include <hypnoticos/memory.h>

void *malloc(size_t size) {
  static void *heap_addr = NULL;
  static uint64_t heap_size = 0;
  uint64_t offset, previous_heap_size;
  malloc_entry_t *entry, *next_entry;
  uint8_t retried = 0;

  if(heap_addr == NULL && heap_size == 0) {
    heap_addr = (void *) KernelFunctionInterface(0, 0, 0, 0, 0, KERNEL_FUNCTION_HEAP_ADDR);
    heap_size = KernelFunctionInterface(0, 0, 0, 0, 0, KERNEL_FUNCTION_HEAP_SIZE);

    if(heap_addr == NULL || heap_size == 0) {
      // TODO Something went wrong
      return NULL;
    }

    entry = (malloc_entry_t *) heap_addr;
    entry->magic = MALLOC_MAGIC;
    entry->size = heap_size - sizeof(malloc_entry_t);
    entry->status = 0;
  } else if(heap_addr == NULL || heap_size == 0) {
    // TODO Something went wrong
    return NULL;
  }

  // Iterate through heap to look for space
retry:
  for(offset = 0; offset < heap_size; offset += sizeof(malloc_entry_t) + entry->size) {
    entry = (malloc_entry_t *) ((uint64_t) heap_addr + offset);

    if(entry->status == 1) {
      continue;
    }

    if(entry->size < size) {
      continue;
    }

    if(entry->size == size) {
      entry->status = 1;
      return (void *) ((uint64_t) entry + sizeof(malloc_entry_t));
    }

    if(entry->size > size + sizeof(malloc_entry_t)) {
      // Insufficient space to split the block but allocate the block anyway
      entry->status = 1;
      return (void *) ((uint64_t) entry + sizeof(malloc_entry_t));
    }

    // Otherwise, the block must be split
    next_entry = (malloc_entry_t *) ((uint64_t) entry + sizeof(malloc_entry_t) + size);
    next_entry->magic = MALLOC_MAGIC;
    next_entry->size = entry->size - sizeof(malloc_entry_t) - size;
    next_entry->status = 0;

    entry->size = size;
    entry->status = 1;
    return (void *) ((uint64_t) entry + sizeof(malloc_entry_t));
  }

  // Reached the end of the heap
  if(retried) {
    // Something went wrong
    return NULL;
  } else {
    // Try to allocate more memory

    // TODO If there is free space at the end of the heap before the new pages are allocated, then that entry must be extended rather than creating a new entry

    if((entry = (malloc_entry_t *) KernelFunctionInterface(size, 0, 0, 0, 0, KERNEL_FUNCTION_NEW_PAGE)) == NULL) {
      return NULL;
    }

    previous_heap_size = heap_size;
    heap_size = KernelFunctionInterface(0, 0, 0, 0, 0, KERNEL_FUNCTION_HEAP_SIZE);

    entry = (malloc_entry_t *) ((uint64_t) heap_addr + previous_heap_size);
    entry->magic = MALLOC_MAGIC;
    entry->size = heap_size - previous_heap_size - sizeof(malloc_entry_t);
    entry->status = 0;

    retried = 1;
    goto retry;
  }
}

#endif
