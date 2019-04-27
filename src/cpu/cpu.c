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

void CpuChecks() {
  uint32_t *r;
  char s[13];

  if(!CpuidTest()) {
    HALT();
  }

  // Vendor identification string -  EBX, EDX, ECX
  r = Cpuid(0x00);
  memcpy(s, &r[1], 4);
  memcpy(s + 4, &r[3], 4);
  memcpy(s + 8, &r[2], 4);
  s[12] = 0;
  printf("Vendor identification string: %s\n", s);

  if(!ApicLocalCheck()) {
    HALT();
  } else if(!AcpiParseApic()) { // This function finds the I/O APIC
    HALT();
  } else if(!ApicIoInit()) {
    HALT();
  }
}
