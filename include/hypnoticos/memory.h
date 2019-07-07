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

#ifndef HYPNOTICOS_MEMORY_H
#define HYPNOTICOS_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define ALIGN_NONE              0
#define ALIGN_4KB               4

#define PAGING_PRESENT          0x1
#define PAGING_RW               0x2
#define PAGING_USER             0x4
#define PAGING_PAGE_FRAME       (0x1 << 7)

#define PAGE_SIZE_4KB           0x1
#define PAGE_SIZE_2MB           0x2
#define PAGE_SIZE_1GB           0x3

#define MALLOC_MAGIC              0xABCDEFABCDEFABCD

typedef struct _malloc_entry_t malloc_entry_t;
struct _malloc_entry_t {
  uint64_t magic; /*!< A magic number to check for a potentially erroneous free(). */
  uint64_t size; /*!< The size of the block on the heap */
  uint8_t status; /*!< 0 = Free, 1 = In use */
};

#define MEMORYBLOCK_TYPE_AVAILABLE        1
#define MEMORYBLOCK_TYPE_UNAVAILABLE      2

#define MEMORY_TABLE_INITIAL_ENTRIES          100       // It may be better to have a smaller number here to make try to avoid a conflict with the kernel/mmap (this is relevant for now - TODO)
#define MEMORY_TABLE_NEW_TABLE_ENTRIES        1000

#define MEMORY_TABLE_FUNCTION_LABEL_SIZE      200

typedef struct _MemoryBlock_t MemoryBlock_t;
struct _MemoryBlock_t {
  uint64_t start; /*!< The start of the memory block */
  uint64_t length; /*!< The length of the memory block */
  uint8_t type; /*!< The memory block's type */

  MemoryBlock_t *prev; /*!< Previous in the linked list (NULL if this is the first entry) */
  MemoryBlock_t *next; /*!< Next in the linked list (NULL if no more entries) */
};
extern MemoryBlock_t MemoryBlocks;

typedef struct _MemoryTableIndex_t MemoryTableIndex_t;
typedef struct _MemoryTable_t MemoryTable_t;
struct _MemoryTableIndex_t {
  MemoryTable_t *addr; /*!< Address of an array of MemoryTable_t structures */
  uint64_t size; /*!< The size of the table */

  MemoryTableIndex_t *prev; /*!< Previous entry in the linked list (NULL if this is the first) */
  MemoryTableIndex_t *next; /*!< Next entry in the linked list (NULL if this is the last) */
} __attribute__((packed));
struct _MemoryTable_t {
  uint64_t addr; /*!< Address of this memory allocation */
  uint64_t size; /*!< Size of this memory allocation */
  uint8_t status; /*!< Status of this entry in the table (0 = empty, 1 = in use) */

  char function[MEMORY_TABLE_FUNCTION_LABEL_SIZE];
  uint32_t line;
} __attribute__((packed));
extern MemoryTableIndex_t MemoryTableIndices;

extern uint64_t MemoryKernelPML4[512];

#define MemoryPagingSetPageImitate(pd, pa, flags, page_size)     MemoryPagingSetPage(pd, pa, pa, flags, page_size)

void *__malloc_align(size_t size, uint8_t align, const char function[MEMORY_TABLE_FUNCTION_LABEL_SIZE], uint32_t line);
uint8_t MemoryAllocated(void *addr, size_t size, const char function[MEMORY_TABLE_FUNCTION_LABEL_SIZE], uint32_t line);
MemoryTable_t *MemoryFind(void *addr);
void *MemoryFindSpace(size_t size, uint8_t align);
void MemoryNewBlock(uint32_t mmap_addr, uint32_t mmap_length, uint64_t start, uint64_t length, uint8_t type);
void MemoryNewTable();
void MemoryPagingInit();
void *MemoryPagingNewPD();
void *MemoryPagingPagePresent(uint64_t *pml4, uint64_t va);
uint8_t MemoryPagingSetPage(uint64_t *pml4, uint64_t va, uint64_t pa, uint32_t flags, uint8_t page_size);

#endif
