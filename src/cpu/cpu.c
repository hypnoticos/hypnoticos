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
#include <hypnoticos/hypnoticos.h>

uint32_t *Cpuid(uint32_t eax_input);
uint32_t CpuidTest();

#define IDT_GATE_COUNT              256

#define IDT_GATE_INTGATE_DEFAULT    0x0E00        // Default flags for an interrupt gate (0b111000000000)
#define IDT_GATE_INTGATE_PRESENT    0x8000        // Present bit (1000000000000000)
#define IDT_GATE_INTGATE_PRIV_0     0x0000
#define IDT_GATE_INTGATE_PRIV_1     0x2000        // 10000000000000
#define IDT_GATE_INTGATE_PRIV_2     0x4000        // 100000000000000
#define IDT_GATE_INTGATE_PRIV_3     0x6000        // 110000000000000

extern void *Stack;

Tss_t Tss;
IdtGate_t IdtGates[IDT_GATE_COUNT] __attribute__((aligned(8))); // TODO Handle IRQs

void TssSet();
void IdtSet(uint16_t limit);
void IdtCreateGate(const uint8_t vector, void (*offset)(), uint16_t ss, uint16_t flags);
void IdtCall(const uint8_t vector, const uint32_t error_code);

void Idt0();
void Idt1();
void Idt2();
void Idt3();
void Idt4();
void Idt5();
void Idt6();
void Idt7();
void Idt8();
void Idt9();
void Idt10();
void Idt11();
void Idt12();
void Idt13();
void Idt14();
void Idt16();
void Idt17();
void Idt18();
void Idt19();
void Idt20();
void IdtReserved();

void TssInit() {
  memset(&Tss, 0, sizeof(Tss_t));

  Tss.ss0 = 0x10;
  Tss.esp0 = (uint32_t) Stack;

  TssSet();
}

void IdtInit() {
  memset(&IdtGates, 0, IDT_GATE_COUNT * sizeof(IdtGate_t));

  IdtCreateGate(0, Idt0, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(1, Idt1, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(2, Idt2, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(3, Idt3, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(4, Idt4, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(5, Idt5, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(6, Idt6, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(7, Idt7, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(8, Idt8, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(9, Idt9, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(10, Idt10, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(11, Idt11, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(12, Idt12, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(13, Idt13, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(14, Idt14, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(15, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(16, Idt16, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(17, Idt17, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(18, Idt18, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(19, Idt19, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(20, Idt20, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(21, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(22, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(23, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(24, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(25, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(26, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(27, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(28, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(29, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(30, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(31, IdtReserved, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);

  IdtSet((IDT_GATE_COUNT * sizeof(IdtGate_t)) - 1);
}

inline void IdtCreateGate(const uint8_t vector, void (*offset)(), uint16_t ss, uint16_t flags) {
  IdtGates[vector].ss = ss;
  IdtGates[vector].flags = (flags | IDT_GATE_INTGATE_DEFAULT);

  IdtGates[vector].offset_low = (uint16_t) ((uint32_t) offset & 0xFFFF);
  IdtGates[vector].offset_high = (uint16_t) ((uint32_t) offset >> 16);
}

void IdtCall(const uint8_t vector, const uint32_t error_code) {
  // TODO
  puts("IdtCall");

  while(1) {
    asm("hlt");
  }
}

void CpuChecks() {
  uint32_t *r;
  char s[13];

  if(!CpuidTest()) {
    HALT();
  }

  // Vendor identification string -  EBX, EDX, ECX
  r = Cpuid(0);
  memcpy(s, &r[1], 4);
  memcpy(s + 4, &r[3], 4);
  memcpy(s + 8, &r[2], 4);
  s[12] = 0;
  printf("Vendor identification string: %s\n", s);
}
