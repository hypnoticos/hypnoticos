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
#include <stdlib.h>
#include <string.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>

#define MSR_IA32_APIC_BASE          0x1B

volatile void *ApicLocalBspBase = NULL;
uint8_t ApInitDone = 0;

uint8_t ApicLocalInit(uint8_t bsp) {
  uint32_t *r;

  if(bsp != CPU_BSP && bsp != CPU_AP) {
    HALT();
  }

  // Check CPUID EAX=1, return value of EDX (bit 9 - APIC)
  r = Cpuid(0x01);
  if(!(r[3] & 0x200)) {
    WARNING();
    return 0;
  }

  // TODO Implement x2APIC support in addition
  // Check CPUID EAX=1, return value of ECX (bit 21 - x2APIC)

  r = MsrRead(MSR_IA32_APIC_BASE);

  if(!(r[1] & 0x800)) {
    // APIC enabled
    return 1;
  } else if(bsp == CPU_BSP && !(r[1] & 0x100)) {
    // BSP not set, and this processor is the BSP
    WARNING();
    return 0;
  } else if(bsp == CPU_AP && (r[1] & 0x100)) {
    // BSP set, and this processor is not the BSP
    WARNING();
    return 0;
  }

  r[1] = r[1] | 0x800;
  MsrWrite(MSR_IA32_APIC_BASE, r[0], r[1]);

  // TODO MAXPHYSADDR
  // TODO Look at this code again. The documents refer to using CPUID, and potentially only part of r[1] should be considered.
  ApicLocalBspBase = (void *) ((uint32_t) r[1] & 0xFFFFF000);

  APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_SIVR, 0x100 | APIC_LOCAL_VECTOR_SPURIOUS);

  return 1;
}

uint8_t ApicLocalParseAcpi(AcpiApicLocal_t *ptr) {
  uint32_t i;
  uint8_t retry = 0;

  // Is this the BSP?
  if(APIC_LOCAL_READ(ApicLocalBspBase, APIC_LOCAL_OFFSET_ID) == ptr->apic_id) {
    return 1;
  }

  if(!(ptr->flags & 0x1) || (ptr->flags & 0x2)) {
    // Not supported
    WARNING();
    return 1;
  }

  ApInitDone = 0;

  APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_ICR_H, ptr->apic_id << 24);
  APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_ICR_L, 0 | (0x5 << 8) | (0x1 << 14));

  // TODO Replace with sleep()
  for(i = 0; i < 0xFFFFFF; i++) {
    asm("pause");
  }

  while(!ApInitDone) {
    APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_ICR_H, ptr->apic_id << 24);
    APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_ICR_L, APIC_LOCAL_VECTOR_AP_START | (0x6 << 8) | (0x1 << 14));

    for(i = 0; i < 0xFFFFFF && ApInitDone == 0; i++) {
      asm("pause");
    }

    if(retry) {
      // Already retried
      WARNING();
      return 0;
    }

    retry = 1;
  }

  return 1;
}

void ApicLocalSetUpTimer() {
  APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_TIMER_DCR, APIC_LOCAL_DCR_2);
  APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_TIMER_ICR, 0xFFFFFFFF);
  APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_TIMER_LVT, APIC_LOCAL_TIMER_PERIODIC | APIC_LOCAL_VECTOR_TIMER);
}

void ApicLocalEoi() {
  APIC_LOCAL_WRITE(ApicLocalBspBase, APIC_LOCAL_OFFSET_EOI, 0x0);
}
