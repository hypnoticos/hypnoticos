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

#include <stdlib.h>
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>

// Retrieve the top 10 bits
#define VA_TO_PD(va)                  ((va) >> 22)

// Get the bottom 10 of the top 20 bits
#define VA_TO_PT(va)                  (((va) >> 12) & 0x3FF)

#define PD_VA_BASE(pd)                (((1024 * 4) * (pd)) << 10)
#define PT_VA_OFFSET(pt)              ((1024 * 4) * (pt))

extern void MemoryPagingEnable(uint32_t cr3);

void *MemoryPD;

void MemoryPagingInit() {
  uint32_t pde, pte, va;

  // Allocate kernel page directory
  MemoryPD = malloc_align(4096, ALIGN_4KB);

  // Allocate every page
  for(pde = 0; pde < 1024; pde++) {
    for(pte = 0; pte < 1024; pte++) {
      va = PD_VA_BASE(pde) | PT_VA_OFFSET(pte);
      MemoryPagingSetPageImitate(MemoryPD, va, PAGING_PRESENT | ((va == (uint32_t) MemoryPD || va == 0x800000) ? 0 : PAGING_RW));
    }
  }

  // Enable paging
  // (the CR3 value to be given as a parameter will already have bits 0 to 11 set to 0 because it is 4KB aligned)
  MemoryPagingEnable((uint32_t) MemoryPD);
}

void *MemoryPagingNewPD() {
  uint32_t pde, start, end, i, *ret;

  // Initialise each PDE
  ret = malloc_align(4096, ALIGN_4KB);
  for(pde = 0; pde < 1024; pde++) {
    ret[pde] = 0;
  }

  // Reserve the kernel's pages
  start = ((uint32_t) &AddrStart) / 4096;
  end = (((uint32_t) &AddrEnd) + 4096) / 4096;
  for(i = start; i < end; i++) {
    MemoryPagingSetPageImitate(ret, i * 4096, PAGING_PRESENT);
  }

  return ret;
}

uint8_t MemoryPagingSetPage(uint32_t *pd, uint32_t va, uint32_t pa, uint32_t flags) {
  uint32_t pde, pte, i;
  void *ptr;

  if((pa & 0xFFF) != 0) {
    WARNING();
    return 0;
  } else if((flags & 0xFFFFF000) != 0) {
    WARNING();
    return 0;
  }

  // Get PDE & PTE
  pde = VA_TO_PD(va);
  pte = VA_TO_PT(va);

  // Check if PDE is allocated
  if(!(pd[pde] & PAGING_PRESENT)) {
    ptr = malloc_align(4096, ALIGN_4KB);
    for(i = 0; i < 1024; i++) {
      *((uint32_t *) ((uint32_t) ptr + (i * 4))) = 0;
    }
    pd[pde] = (uint32_t) ptr | PAGING_PRESENT | PAGING_RW | PAGING_USER;
  }

  // Set PTE entry
  ptr = (void *) ((uint32_t) (pd[pde] & 0xFFFFF000) + (pte * 4));
  *((uint32_t *) ptr) = pa | flags;

  return 1;
}
