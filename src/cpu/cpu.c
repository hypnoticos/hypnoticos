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
#include <stdint.h>
#include <string.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/devices.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>

uint32_t CpuidTest();
uint8_t CpuCheckVendorIdString();

extern void *IdtStackTop;

Tss_t Tss;

void TssSet();

void TssInit() {
  memset(&Tss, 0, sizeof(Tss_t));

  Tss.ss0 = 0x10;
  Tss.esp0 = (uint32_t) &IdtStackTop;

  Tss.iomap_base = (uint32_t) &Tss.io_permission - (uint32_t) &Tss;

  memset(&Tss.int_redirection, 0, sizeof(Tss.int_redirection));
  memset(&Tss.io_permission, 0xFF, sizeof(Tss.io_permission));
  Tss.io_permission_final = 0xFF;

  TssSet();
}

uint8_t CpuCheckVendorIdString() {
  char s[13];
  uint32_t *r;

  // Check vendor identification string -  EBX, EDX, ECX
  r = Cpuid(0x00);
  memcpy(s, &r[1], 4);
  memcpy(s + 4, &r[3], 4);
  memcpy(s + 8, &r[2], 4);
  s[12] = 0;
  if(strcmp(s, "GenuineIntel") != 0 && strcmp(s, "AuthenticAMD") != 0) {
    WARNING();
    return 0;
  }

  return 1;
}

void CpuChecks(uint8_t bsp) {
  if(!CpuidTest()) {
    HALT();
  } else if(!CpuCheckVendorIdString()) {
    HALT();
  } else if(!ApicLocalInit(bsp)) {
    HALT();
  }

  if(bsp == CPU_BSP) {
    if(!AcpiParse()) { // This function finds the I/O APIC
      HALT();
    } else if(!ApicIoInit()) {
      HALT();
    }
  }
}
