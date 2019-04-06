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

extern void MemoryPagingEnable(uint32_t cr3);

void *MemoryPD;

void MemoryPagingInit() {
  uint32_t pde, pte;

  // Allocate kernel page directory
  MemoryPD = malloc_align(4096, ALIGN_4KB);

  // Allocate every page
  for(pde = 0; pde < 1024; pde++) {
    for(pte = 0; pte < 1024; pte++) {
      MemoryPagingSetPage(MemoryPD, (pde * 0x400000) + (pte * 4096), PAGING_PRESENT | PAGING_RW);
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
    MemoryPagingSetPage(ret, i * 4096, PAGING_PRESENT | PAGING_RW);
  }

  return ret;
}

uint8_t MemoryPagingSetPage(uint32_t *pd, uint32_t addr, uint32_t flags) {
  uint32_t pde, pte, i;

  if((addr & 0xFFF) != 0) {
    return 0;
  } else if((flags & 0xFFFFF000) != 0) {
    return 0;
  }

  // Get PDE & PTE
  pde = addr / 0x400000;
  pte = (addr % 0x400000) / 4096;

  // Check if PDE is allocated
  if(!(pd[pde] & 0x1)) {
    pd[pde] = (uint32_t) malloc_align(4096, ALIGN_4KB);
    for(i = 0; i < 1024; i++) {
      // This should be the same as: *((uint32_t *)((pd[pde] & 0xFFFFF000) + (4 * i2))) = ...
      *((uint32_t *) (pd[pde] + (4 * i))) = 0;
    }
    pd[pde] = pd[pde] | PAGING_PRESENT | PAGING_RW | PAGING_USER;
  }

  // Set PTE entry
  *((uint32_t *) ((pd[pde] & 0xFFFFF000) + (4 * pte))) = addr | flags;

  return 1;
}
