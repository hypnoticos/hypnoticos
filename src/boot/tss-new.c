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

uint16_t TssTotal = 1;
uint16_t TssLast = 0;
extern uint32_t GdtEntries;

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

uint8_t TssNew() {
  void *stack;
  Tss_t *new_tss;
  GdtTss_t *gdt_tss;

  if(TssTotal >= 255) {
    return 0;
  }

  // Create a new stack
  stack = malloc_align(8192, ALIGN_4KB);

  // Create a new TSS
  new_tss = malloc(sizeof(Tss_t));
  memset(new_tss, 0, sizeof(Tss_t));
  new_tss->rsp0 = (uint64_t) stack;

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
