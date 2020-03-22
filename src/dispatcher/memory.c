//
// HypnoticOS
// Copyright (C) 2019, 2020  jk30
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
#include <stdlib.h>
#include <string.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>

uint8_t inline DispatcherProcessCheckVa(DispatcherProcess_t *p, uint64_t va, uint8_t kernel_function_ignore);

uint8_t inline DispatcherProcessCheckVa(DispatcherProcess_t *p, uint64_t va, uint8_t kernel_function_ignore) {
  uint32_t i;

  for(i = 0; p->va[i] != NULL; i++) {
    if(p->va[i]->va == va) {
      if(kernel_function_ignore == 1 && p->va[i]->ignore == 1) {
        // Found, but to be ignored
        return 2;
      } else {
        // Found
        return 1;
      }
    }
  }

  return 0;
}

uint8_t DispatcherProcessMap(DispatcherProcess_t *p, uint64_t va, uint64_t pa, uint8_t kernel_function_ignore, uint32_t flags) {
  uint64_t i;

  // Check if va and pa are 4KB aligned
  if((va & 0xFFF) || (pa & 0xFFF)) {
    WARNING();
    return 0;
  }

  // Check for duplicates in p->va
  i = DispatcherProcessCheckVa(p, va, kernel_function_ignore);
  if(i != 0) {
    WARNING();
    return 0;
  }

  // Add to p->va
  for(i = 0; p->va[i] != NULL; i++);
  p->va = realloc(p->va, sizeof(DispatcherProcessVa_t *) * (i + 2));
  p->va[i] = malloc(sizeof(DispatcherProcessVa_t));
  p->va[i + 1] = NULL;

  p->va[i]->ignore = kernel_function_ignore;
  p->va[i]->va = va;
  p->va[i]->pa = (void *) pa;

  return MemoryPagingSetPage((uint64_t *) p->save.cr3, va, pa, flags, PAGE_SIZE_4KB);
}

void *DispatcherProcessGetPa(DispatcherProcess_t *p, uint64_t va, uint8_t ignore) {
  uint64_t i, va_range, offset;

  va_range = va & 0xFFFFFFFFFFFFF000;
  offset = va & 0xFFF;

  for(i = 0; p->va[i] != NULL; i++) {
    if(p->va[i]->va == va_range) {
      if(ignore == 1 && p->va[i]->ignore == 1) {
        // Found, but to be ignored
        WARNING();
        return NULL;
      } else {
        // Found
        return p->va[i]->pa + offset;
      }
    }
  }

  // Not found
  WARNING();
  return NULL;
}

void *DispatcherProcessAllocatePage(DispatcherProcess_t *p, uint64_t va, uint8_t kernel_function_ignore, uint32_t flags) {
  uint32_t i;
  void *ptr;

  ptr = malloc_align(4096, ALIGN_4KB);
  if(ptr == NULL) {
    WARNING();
    return NULL;
  }

  if(!DispatcherProcessMap(p, va, (uint64_t) ptr, kernel_function_ignore, flags)) {
    free(ptr);
    WARNING();
    return NULL;
  }

  for(i = 0; p->alloc[i] != NULL; i++);

  p->alloc = realloc(p->alloc, sizeof(void *) * (i + 2));
  p->alloc[i] = ptr;
  p->alloc[i + 1] = NULL;

  memset(ptr, 0, 4096);

  return ptr;
}

uint8_t DispatcherProcessLoadAt(DispatcherProcess_t *p, uint64_t va, char *data, uint64_t file_size, uint64_t memory_size, uint32_t flags) {
  uint64_t i, memory_count, file_count, page_min_addr, page_max_addr, pages, initial_offset;
  void **ptrs;

  memory_count = (memory_size / 4096) + 1;
  file_count = (file_size / 4096) + 1;
  if(memory_count < file_count) {
    WARNING();
    return 0;
  }

  page_min_addr = va & 0xFFFFFFFFFFFFF000;
  page_max_addr = (va + memory_size) | 0xFFF;
  pages = ((page_max_addr + 1) - page_min_addr) / 4096;

  ptrs = malloc(sizeof(void *) * pages);
  for(i = 0; i < pages; i++) {
    // TODO Check if page is already allocated.

    if((ptrs[i] = DispatcherProcessAllocatePage(p, page_min_addr + (i * 4096), 0, flags)) == NULL) {
      // TODO Clean up
      WARNING();
      return 0;
    }
  }

  if(file_size != 0) {
    initial_offset = va & 0xFFF;
    for(i = 0; i < file_count; i++) {
      if(i == 0) {
        memcpy(ptrs[i] + initial_offset, data, 4096 - initial_offset);
      } else if(i != file_count - 1) {
        memcpy(ptrs[i], &data[(i * 4096) - (4096 - initial_offset)], 4096);
      } else {
        memcpy(ptrs[i], &data[(i * 4096) - (4096 - initial_offset)], file_size % 4096);
      }
    }
  }

  return 1;
}
