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

AcpiApicLocal_t **ApicLocal = NULL;
void *ApicLocalBspBase = NULL;

void ApicLocalEnable();
void ApicLocalWrite(void *base_addr, uint32_t offset, uint32_t value);

uint8_t ApicLocalCheck() {
  uint32_t *r;

  // Check CPUID EAX=1, return value of EDX (bit 9 - APIC)
  r = Cpuid(0x01);
  if(!(r[3] & 0x200)) {
    return 0;
  }

  // TODO Implement x2APIC support in addition
  // Check CPUID EAX=1, return value of ECX (bit 21 - x2APIC)

  ApicLocalEnable();
  return 1;
}

void ApicLocalEnable() {
  uint32_t *r;

  r = MsrRead(MSR_IA32_APIC_BASE);

  if((r[1] & 0x800)) {
    // APIC not enabled
    r[1] = r[1] | 0x800;
    MsrWrite(MSR_IA32_APIC_BASE, r[0], r[1]);
  } else if(!(r[1] & 0x100)) {
    // BSP not set
    HALT();
  }

  // TODO MAXPHYSADDR
  // TODO Look at this code again. The documents refer to using CPUID, and potentially only part of r[1] should be considered.
  ApicLocalBspBase = (void *) ((uint32_t) r[1] & 0xFFFFF000);

  ApicLocalWrite(ApicLocalBspBase, APIC_LOCAL_OFFSET_SIVR, 0x100 | APIC_LOCAL_VECTOR_SPURIOUS);
}

uint8_t ApicLocalAdd(AcpiApicLocal_t *ptr) {
  uint32_t i;

  if(ApicLocal == NULL) {
    ApicLocal = malloc(sizeof(AcpiApicLocal_t *) * 2);
    i = 0;
  } else {
    for(i = 0; ApicLocal[i] != NULL; i++);
    ApicLocal = realloc(ApicLocal, sizeof(AcpiApicLocal_t *) * (i + 2));
  }

  ApicLocal[i] = malloc(sizeof(AcpiApicLocal_t));
  memcpy(ApicLocal[i], ptr, sizeof(AcpiApicLocal_t));

  // TODO Process flags

  return 1;
}

void ApicLocalSetUpTimer() {
  ApicLocalWrite(ApicLocalBspBase, APIC_LOCAL_OFFSET_TIMER_DCR, APIC_LOCAL_DCR_2);
  ApicLocalWrite(ApicLocalBspBase, APIC_LOCAL_OFFSET_TIMER_LVT, 0x0 | APIC_LOCAL_VECTOR_TIMER);
  ApicLocalWrite(ApicLocalBspBase, APIC_LOCAL_OFFSET_TIMER_ICR, 0x2);
}

void ApicLocalWrite(void *base_addr, uint32_t offset, uint32_t value) {
  *((uint32_t *) ((uint32_t) base_addr + offset)) = value;
}
