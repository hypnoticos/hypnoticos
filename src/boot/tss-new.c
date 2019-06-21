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
#include <hypnoticos/boot.h>

typedef struct _GdtTss_t GdtTss_t;
struct _GdtTss_t {
  uint16_t length;
  uint16_t base_0_15;
  uint8_t base_16_23;
  uint8_t middle_1;
  uint8_t middle_2;
  uint8_t base_24_31;
  uint32_t base_32_63;
  uint32_t zero;
};

uint16_t TssTotal = 1;
uint16_t TssLast = 0;
TssEntries_t **TssEntriesAPs = NULL;
extern uint32_t GdtEntries;

uint8_t TssNew() {
  void *stack;
  Tss_t *new_tss;
  GdtTss_t *gdt_tss;
  uint64_t tss_pages;

  if(TssTotal >= 255) {
    return 0;
  }

  // Create a new stack
  stack = malloc_align(TSS_RSP0_SIZE, ALIGN_4KB);
  memset(stack, 0, TSS_RSP0_SIZE);

  // Create a new TSS
  tss_pages = (sizeof(Tss_t) / 4096) + 1;
  new_tss = malloc_align(tss_pages * 4096, ALIGN_4KB);
  memset(new_tss, 0, sizeof(Tss_t));
  new_tss->rsp0 = (uint64_t) stack + TSS_RSP0_SIZE - 8;

  if(TssEntriesAPs == NULL) {
    TssEntriesAPs = malloc(sizeof(Tss_t *) * 2);
    TssEntriesAPs[0] = malloc(sizeof(TssEntries_t));
    TssEntriesAPs[0]->tss = new_tss;
    TssEntriesAPs[0]->stack = stack;
    TssEntriesAPs[1] = NULL;
  } else {
    TssEntriesAPs = realloc(TssEntriesAPs, sizeof(Tss_t *) * (TssTotal + 1));
    TssEntriesAPs[TssTotal - 1] = malloc(sizeof(TssEntries_t));
    TssEntriesAPs[TssTotal - 1]->tss = new_tss;
    TssEntriesAPs[TssTotal - 1]->stack = stack;
    TssEntriesAPs[TssTotal] = NULL;
  }

  // Add to the GDT entry
  gdt_tss = (GdtTss_t *) ((uint64_t) &GdtEntries + (5 * 8) + (TssTotal * 16));
  gdt_tss->base_0_15 = ((uint64_t) new_tss & 0xFFFF);
  gdt_tss->base_16_23 = (((uint64_t) new_tss >> 16) & 0xFF);
  gdt_tss->base_24_31 = (((uint64_t) new_tss >> 23) & 0xFF);
  gdt_tss->base_32_63 = ((uint64_t) new_tss >> 32);
  gdt_tss->length = 0x2089;
  gdt_tss->middle_1 = 0x89;
  gdt_tss->middle_2 = 0;

  TssLast = 0x08 * (5 + (TssTotal * 2));
  TssTotal++;

  return 1;
}
