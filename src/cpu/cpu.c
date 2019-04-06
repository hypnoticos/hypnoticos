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
void Idt48();
void Idt49();
void Idt51();
void Idt52();
void Idt53();
void Idt54();
void Idt55();
void Idt56();
void Idt57();
void Idt58();
void Idt59();
void Idt60();
void Idt61();
void Idt62();
void Idt63();
void Idt64();
void Idt65();
void Idt66();
void Idt67();
void Idt68();
void Idt69();
void Idt70();
void Idt71();
void Idt160();
void Idt240();
void IdtReserved();

void TssInit() {
  memset(&Tss, 0, sizeof(Tss_t));

  Tss.ss0 = 0x10;
  Tss.esp0 = (uint32_t) &Stack;

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

  /// Local APIC interrupts
  // NOTE: take care when renumbering these IDT entries
  IdtCreateGate(APIC_LOCAL_VECTOR_TIMER, Idt160, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(APIC_LOCAL_VECTOR_SPURIOUS, Idt240, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);

  // IO APIC interrupts
  IdtCreateGate(0x30 + 0, Idt48, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(IDT_IRQ_1, Idt49, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(IDT_IRQ_2, DispatcherInterrupt, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 3, Idt51, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 4, Idt52, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 5, Idt53, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 6, Idt54, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 7, Idt55, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 8, Idt56, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 9, Idt57, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 10, Idt58, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 11, Idt59, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 12, Idt60, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 13, Idt61, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 14, Idt62, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 15, Idt63, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 16, Idt64, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 17, Idt65, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 18, Idt66, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 19, Idt67, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 20, Idt68, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 21, Idt69, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 22, Idt70, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
  IdtCreateGate(0x30 + 23, Idt71, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);

  IdtSet((IDT_GATE_COUNT * sizeof(IdtGate_t)) - 1);
}

inline void IdtCreateGate(const uint8_t vector, void (*offset)(), uint16_t ss, uint16_t flags) {
  IdtGates[vector].ss = ss;
  IdtGates[vector].flags = (flags | IDT_GATE_INTGATE_DEFAULT);

  IdtGates[vector].offset_low = (uint16_t) ((uint32_t) offset & 0xFFFF);
  IdtGates[vector].offset_high = (uint16_t) ((uint32_t) offset >> 16);
}

void IdtCall(const uint8_t vector, const uint32_t error_code) {
  switch(vector) {
    case APIC_LOCAL_VECTOR_TIMER:
    // TODO Call process manager
    //ApicLocalSetUpTimer(); // TODO Reset the timer
    ApicLocalEoi();
    break;

    case APIC_LOCAL_VECTOR_SPURIOUS:
    printf("\nSPURIOUS INTERRUPT\n");
    while(1) {
      asm("hlt");
    }
    break;

    case 48:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    printf("IRQ%u\n", vector - 48);
    ApicLocalEoi();
    break;

    case IDT_IRQ_1:
    KeyboardIrq();
    ApicLocalEoi();
    break;

    // IRQ 2 is the timer and is sent to the dispatcher instead of this function

    default:
    printf("\nUNKNOWN INTERRUPT");
    while(1) {
      asm("hlt");
    }
    break;
  }
}

void IdtCallException(const uint8_t vector, const uint32_t cr2, const uint32_t eip, const uint16_t cs, const uint32_t error_code) {
  const static char *descriptions[] = {
    "#DE, Divide Error",                                // 0
    "#DB, Debug Exception",                             // 1
    "NMI Interrupt",                                    // 2
    "#BP, Breakpoint",                                  // 3
    "#OF, Overflow",                                    // 4
    "#BR, BOUND Range Exceeded",                        // 5
    "#UD, Invalid Opcode (Undefined Opcode)",           // 6
    "#NM, Device Not Available (No Math Coprocessor)",  // 7
    "#DF, Double Fault",                                // 8
    "Coprocessor Segment Overrun",                      // 9
    "#TS, Invalid TSS",                                 // 10
    "#NP, Segment Not Present",                         // 11
    "#SS, Stack-Segment Fault",                         // 12
    "#GP, General Protection",                          // 13
    "#PF, Page Fault",                                  // 14
    "(unknown)",                                        // 15 and 21-31
    "#MF, x87 FPU Floating-Point Error (Math Fault)",   // 16
    "#AC, Alignment Check",                             // 17
    "#MC, Machine Check",                               // 18
    "#XM, SIMD Floating-Point Exception",               // 19
    "#VE, Virtualization Exception"                     // 20
  };

  printf("\n");
  printf("INTERRUPT: VECTOR %u - %s\n", vector, descriptions[vector]);
  printf("* Error code = 0x%X\n", error_code);

  if(vector == 14) {
    printf("  Page fault details: P=%u, W/R=%u, U/S=%u, RSVD=%u, I/D=%u, PK=%u, SGX=%u\n",
      /* P */     (error_code & 0x1) != 0,
      /* W/R */   (error_code & 0x2) != 0,
      /* U/S */   (error_code & 0x4) != 0,
      /* RSVD */  (error_code & 0x8) != 0,
      /* I/D */   (error_code & 0x10) != 0,
      /* PK */    (error_code & 0x20) != 0,
      /* SGX */   (error_code & 0x8000) != 0);
  }

  printf("* CS         = 0x%X\n", cs);
  printf("* EIP        = 0x%X\n", eip);
  printf("* CR2        = 0x%X\n", cr2);

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
