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
#include <string.h>
#include <hypnoticos/memory.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>

#define PAGE_ENTRY_SIZE               (512 * sizeof(uint64_t))

#define VA_GET_PML4_ENTRY(va)         ((va) >> 39)
#define VA_GET_PDPTE(va)              (((va) >> 30) & 0x1FF)
#define VA_GET_PDE(va)                (((va) >> 21) & 0x1FF)
#define VA_GET_PTE(va)                (((va) >> 12) & 0x1FF)

uint8_t MemoryPagingPageChangeFlags(uint64_t *pml4, uint64_t va, uint32_t flags_unset, uint32_t flags_set);

extern void *TextMainStart, *TextMainEnd;

void MemoryPagingInit() {
  uint64_t max_addr = 0, max_page, i;

  for(MemoryBlock_t *mb = &MemoryBlocks; mb != NULL; mb = mb->next) {
    if((uint64_t) mb->start + (uint64_t) mb->length > max_addr) {
      max_addr = (uint64_t) mb->start + (uint64_t) mb->length;
    }
  }

  // Allocate remaining pages in 2MB blocks
  max_page = (max_addr / 0x200000) + 1;
  for(i = 0; i < max_page; i++) {
    if(MemoryPagingPagePresent(MemoryKernelPML4, i * 0x200000) == NULL) {
      if(!MemoryPagingSetPageImitate(MemoryKernelPML4, i * 0x200000, PAGING_PRESENT | PAGING_RW, PAGE_SIZE_2MB)) {
        HALT();
      }
    }
  }

  i = ((uint64_t) &TextMainStart) / 0x200000;
  max_page = (((uint64_t) &TextMainEnd) / 0x200000) + 1;
  for(; i < max_page; i++) {
    MemoryPagingPageChangeFlags(MemoryKernelPML4, i * 0x200000, PAGING_RW, 0);
  }
}

void *MemoryPagingNewPD() {
  uint32_t flag_rw;
  uint64_t start, end, text_start, text_end, i, i2, *ret;

  ret = malloc_align(4096, ALIGN_4KB);
  memset(ret, 0, 4096);

  // Reserve the kernel's pages
  start = ((uint64_t) &AddrStart) / 4096;
  end = (((uint64_t) &AddrEnd) + 4096) / 4096;
  text_start = ((uint64_t) &TextMainStart) / 4096;
  text_end = (((uint64_t) &TextMainEnd) / 4096) + 1;
  for(i = start; i < end; i++) {
    flag_rw = PAGING_RW;
    if(i >= text_start && i <= text_end) {
      flag_rw = 0;
    }
    MemoryPagingSetPageImitate(ret, i * 4096, PAGING_PRESENT | flag_rw, PAGE_SIZE_4KB);
  }

  // Reserve the pages needed for the TSS
  for(i = 0; TssEntriesAPs[i] != NULL; i++) {
    start = ((uint64_t) TssEntriesAPs[i]->tss) / 4096;
    end = (((uint64_t) TssEntriesAPs[i]->tss + sizeof(Tss_t)) / 4096) + 1;
    for(i2 = start; i2 < end; i2++) {
      MemoryPagingSetPageImitate(ret, i2 * 4096, PAGING_PRESENT | PAGING_RW, PAGE_SIZE_4KB);
    }

    start = ((uint64_t) TssEntriesAPs[i]->stack) / 4096;
    end = (((uint64_t) TssEntriesAPs[i]->stack + TSS_RSP0_SIZE) / 4096) + 1;
    for(i2 = start; i2 < end; i2++) {
      MemoryPagingSetPageImitate(ret, i2 * 4096, PAGING_PRESENT | PAGING_RW, PAGE_SIZE_4KB);
    }
  }

  return ret;
}

uint8_t MemoryPagingPageChangeFlags(uint64_t *pml4, uint64_t va, uint32_t flags_unset, uint32_t flags_set) {
  uint64_t *ptr;
  uint32_t current_flags, new_flags;

  if((ptr = MemoryPagingPagePresent(pml4, va)) == NULL) {
    return 0;
  }

  INFO("ptr=0x%p", ptr);

  current_flags = (*ptr & 0xFFF);
  new_flags = current_flags & (~flags_unset);
  new_flags = new_flags | flags_set;

  *ptr = (*ptr & (~0xFFF)) | new_flags;

  return 1;
}

void *MemoryPagingPagePresent(uint64_t *pml4, uint64_t va) {
  uint64_t **pdpte_ptr, **pde_ptr, **pte_ptr;
  uint16_t pml4e, pdpte, pde, pte;

  pml4e = VA_GET_PML4_ENTRY(va);
  pdpte = VA_GET_PDPTE(va);
  pde = VA_GET_PDE(va);
  pte = VA_GET_PTE(va);

  // Check if PML4E is allocated
  if((pml4[pml4e] & PAGING_PRESENT)) {
    pdpte_ptr = (uint64_t **) (pml4[pml4e] & 0xFFFFFFFFFFFFF000);
  } else {
    return NULL;
  }

  if((((uint64_t) pdpte_ptr[pdpte]) & PAGING_PRESENT) && (((uint64_t) pdpte_ptr[pdpte]) & PAGING_PAGE_FRAME)) {
    return 1;
  } else if(!(((uint64_t) pdpte_ptr[pdpte]) & PAGING_PRESENT)) {
    return NULL;
  } else {
    pde_ptr = (uint64_t **) ((uint64_t) pdpte_ptr[pdpte] & 0xFFFFFFFFFFFFF000);
  }

  if((((uint64_t) pde_ptr[pde]) & PAGING_PRESENT) && (((uint64_t) pde_ptr[pde]) & PAGING_PAGE_FRAME)) {
    return 1;
  } else if(!(((uint64_t) pde_ptr[pde]) & PAGING_PRESENT)) {
    return NULL;
  } else {
    pte_ptr = (uint64_t **) ((uint64_t) pde_ptr[pde] & 0xFFFFFFFFFFFFF000);
  }

  if(((uint64_t) pte_ptr[pte] & PAGING_PRESENT)) {
    return 1;
  } else {
    return NULL;
  }
}

uint8_t inline MemoryPagingSetPage(uint64_t *pml4, uint64_t va, uint64_t pa, uint32_t flags, uint8_t page_size) {
  uint64_t **pdpte_ptr, **pde_ptr, **pte_ptr;
  uint16_t pml4e, pdpte, pde, pte;

  if(page_size == PAGE_SIZE_4KB && ((va & 0xFFF) != 0 || (pa & 0xFFF) != 0)) {
    WARNING();
    return 0;
  } else if(page_size == PAGE_SIZE_2MB && ((va & 0x1FFFFF) != 0 || (pa & 0x1FFFFF) != 0)) {
    WARNING();
    return 0;
  } else if(page_size == PAGE_SIZE_1GB && ((va & (0x3FFFFFFF)) != 0 || (pa & 0x3FFFFFFF) != 0)) {
    WARNING();
    return 0;
  }

  if(page_size != PAGE_SIZE_4KB && page_size != PAGE_SIZE_2MB && page_size != PAGE_SIZE_1GB) {
    // Unknown page size
    WARNING();
    return 0;
  }

  pml4e = VA_GET_PML4_ENTRY(va);
  pdpte = VA_GET_PDPTE(va);
  pde = VA_GET_PDE(va);
  pte = VA_GET_PTE(va);

  // Check if PML4E is allocated
  if((pml4[pml4e] & PAGING_PRESENT)) {
    pdpte_ptr = (uint64_t **) (pml4[pml4e] & 0xFFFFFFFFFFFFF000);
  } else {
    // Allocate PDPTE
    pml4[pml4e] = (uint64_t) malloc_align(PAGE_ENTRY_SIZE, ALIGN_4KB);
    pdpte_ptr = (uint64_t **) pml4[pml4e];

    // Set default flags
    pml4[pml4e] = ((uint64_t) pml4[pml4e] | PAGING_PRESENT | PAGING_RW | PAGING_USER);

    // Clear PDPTE
    memset(pdpte_ptr, 0, PAGE_ENTRY_SIZE);
  }

  if(page_size == PAGE_SIZE_1GB) {
    if((((uint64_t) pdpte_ptr[pdpte]) & PAGING_PRESENT)) {
      // Already present
      WARNING();
      return 0;
    } else {
      pdpte_ptr[pdpte] = (uint64_t *) (pa | flags | PAGING_PAGE_FRAME);
      return 1;
    }
  } else {
    if((((uint64_t) pdpte_ptr[pdpte]) & PAGING_PRESENT) && (((uint64_t) pdpte_ptr[pdpte]) & PAGING_PAGE_FRAME)) {
      WARNING();
      return 0;
    } else if((((uint64_t) pdpte_ptr[pdpte]) & PAGING_PRESENT) && !(((uint64_t) pdpte_ptr[pdpte]) & PAGING_PAGE_FRAME)) {
      pde_ptr = (uint64_t **) ((uint64_t) pdpte_ptr[pdpte] & 0xFFFFFFFFFFFFF000);
    } else {
      // Allocate PDE
      pdpte_ptr[pdpte] = malloc_align(PAGE_ENTRY_SIZE, ALIGN_4KB);
      pde_ptr = (uint64_t **) pdpte_ptr[pdpte];

      // Set default flags
      pdpte_ptr[pdpte] = (uint64_t *) ((uint64_t) pdpte_ptr[pdpte] | PAGING_PRESENT | PAGING_RW | PAGING_USER);

      // Clear PDPTE
      memset(pde_ptr, 0, PAGE_ENTRY_SIZE);
    }
  }

  if(page_size == PAGE_SIZE_2MB) {
    if((((uint64_t) pde_ptr[pde]) & PAGING_PRESENT)) {
      // Already present
      WARNING();
      return 0;
    } else {
      pde_ptr[pde] = (uint64_t *) (pa | flags | PAGING_PAGE_FRAME);
      return 1;
    }
  } else {
    if((((uint64_t) pde_ptr[pde]) & PAGING_PRESENT) && (((uint64_t) pde_ptr[pde]) & PAGING_PAGE_FRAME)) {
      WARNING();
      return 0;
    } else if((((uint64_t) pde_ptr[pde]) & PAGING_PRESENT) && !(((uint64_t) pde_ptr[pde]) & PAGING_PAGE_FRAME)) {
      pte_ptr = (uint64_t **) ((uint64_t) pde_ptr[pde] & 0xFFFFFFFFFFFFF000);
    } else {
      // Allocate PTE
      pde_ptr[pde] = malloc_align(PAGE_ENTRY_SIZE, ALIGN_4KB);
      pte_ptr = (uint64_t **) pde_ptr[pde];

      // Set default flags
      pde_ptr[pde] = (uint64_t *) ((uint64_t) pde_ptr[pde] | PAGING_PRESENT | PAGING_RW | PAGING_USER);

      // Clear PDPTE
      memset(pte_ptr, 0, PAGE_ENTRY_SIZE);
    }
  }

  // Page size is therefore 4KB
  pte_ptr[pte] = (uint64_t *) (pa | flags);
  return 1;
}
