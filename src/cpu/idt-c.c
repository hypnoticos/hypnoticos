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
#include <string.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/function.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/devices.h>

#define IDT_GATE_COUNT              256

#define IDT_GATE_INTGATE_DEFAULT    0x0E        // Default flags for an interrupt gate
#define IDT_GATE_INTGATE_PRESENT    0x80
#define IDT_GATE_INTGATE_PRIV_0     0x00
#define IDT_GATE_INTGATE_PRIV_1     0x20
#define IDT_GATE_INTGATE_PRIV_2     0x40
#define IDT_GATE_INTGATE_PRIV_3     0x60

IdtGate_t IdtGates[IDT_GATE_COUNT] __attribute__((aligned(8)));
extern uint32_t IdtCallVector;
extern uint16_t IdtLimit;
extern uint64_t IdtBase;
uint8_t IdtFull = 0;
extern uint16_t IdtCallCs;
extern uint8_t IdtCallErrorCode;
extern uint64_t IdtCallErrorCodeOnStack;

uint64_t __attribute__((aligned(8))) IdtTicks = 0;

extern void Idt0();
extern void Idt1();
extern void Idt2();
extern void Idt3();
extern void Idt4();
extern void Idt5();
extern void Idt6();
extern void Idt7();
extern void Idt8();
extern void Idt9();
extern void Idt10();
extern void Idt11();
extern void Idt12();
extern void Idt13();
extern void Idt14();
extern void Idt16();
extern void Idt17();
extern void Idt18();
extern void Idt19();
extern void Idt20();
extern void Idt48();
extern void Idt49();
extern void Idt50();
extern void Idt51();
extern void Idt52();
extern void Idt53();
extern void Idt54();
extern void Idt55();
extern void Idt56();
extern void Idt57();
extern void Idt58();
extern void Idt59();
extern void Idt60();
extern void Idt61();
extern void Idt62();
extern void Idt63();
extern void Idt64();
extern void Idt65();
extern void Idt66();
extern void Idt67();
extern void Idt68();
extern void Idt69();
extern void Idt70();
extern void Idt71();
extern void Idt160();
extern void Idt240();
extern void Idt241();
extern void Idt242();
extern void IdtReserved();
void IdtCreateGate(const uint8_t vector, void (*offset)(), uint16_t cs, uint8_t flags);
void IdtCall();

void IdtCall() {
  DispatcherProcess_t *p;
  uint8_t local_apic_id;
  DispatcherCpu_t *dispatcher_cpu_entry;
  const static char *exception_descriptions[] = {
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
  const static char *level_descriptions[] = {
    "Kernel level",
    "(unknown)",
    "(unknown)",
    "Process level"
  };

  // TODO Handle IRQs
  if(IdtCallVector <= 31) {
    uint8_t privilege_level = (IdtCallCs & 0x3);

    printf("\n");
    printf("**** EXCEPTION ****\n");
    printf("Vector: %u (%s)\n", IdtCallVector,
      exception_descriptions[(IdtCallVector > 20 ? 15 : IdtCallVector)]);

    if(IdtCallErrorCode) {
      printf("Error code: 0x%X\n", IdtCallErrorCodeOnStack);
    } else {
      printf("Error code: (none)\n");
    }
    printf("Exception occurred at: %s\n", level_descriptions[privilege_level]);
    printf("Saved registers:\n");
    printf("  rsp=0x%X rbp=0x%X\n", IdtCallSavedRsp, IdtCallSavedRbp);
    printf("  rax=0x%X rbx=0x%X\n", IdtCallSavedRax, IdtCallSavedRbx);
    printf("  rcx=0x%X rdx=0x%X\n", IdtCallSavedRcx, IdtCallSavedRdx);
    printf("  rsi=0x%X rdi=0x%X\n", IdtCallSavedRsi, IdtCallSavedRdi);
    printf("  rip=0x%X rflags=0x%X\n", IdtCallSavedRip, IdtCallSavedRflags);
    printf("  r8=0x%X r9=0x%X\n", IdtCallSavedR8, IdtCallSavedR9);
    printf("  r10=0x%X r11=0x%X\n", IdtCallSavedR10, IdtCallSavedR11);
    printf("  r12=0x%X r13=0x%X\n", IdtCallSavedR12, IdtCallSavedR13);
    printf("  r14=0x%X r15=0x%X\n", IdtCallSavedR14, IdtCallSavedR15);

    if(privilege_level != 3) {
      printf("The exception occurred at non-process level, so the kernel will be halted.\n");
halt:
      HALT();
      while(1) {
        asm("hlt");
      }
    } else {
      DispatcherCpu_t *dispatcher_cpu;
      DispatcherProcess_t *p;
      printf("The exception occurred at process level, so the process will be terminated.\n");

      dispatcher_cpu = DispatcherGetCpu(APIC_LOCAL_GET_ID());

      if(dispatcher_cpu->current_pid != 0 && (p = DispatcherFind(dispatcher_cpu->current_pid)) != NULL) {
        printf("Terminating PID %u (%s)\n", p->pid, p->name);
        DispatcherProcessDone(p);
        dispatcher_cpu->current_pid = 0;
        IdtWait();
        __builtin_unreachable();
      } else {
        printf("Error terminating process.\n");
        goto halt;
      }
    }
  } else if(IdtCallVector == APIC_LOCAL_VECTOR_TIMER) {
    if(APIC_LOCAL_GET_ID() == ApicLocalBspId) {
      // TODO Implement a more reliable counter

      // Increase ticks
      IdtTicks++;
    }

    DispatcherSetUpNext(APIC_LOCAL_GET_ID());

    if(!IdtFull) {
      // Now that the local APIC has been called, map all IRQs
      INFO("ApicIoMapIrqs()");
      ApicIoMapIrqs();
      IdtFull = 1;

      // And start interrupts on APs
      INFO("Starting interrupts on APs");
      ApicLocalStartInterruptsOnAPs();
    }
  } else if(IdtCallVector == APIC_LOCAL_VECTOR_SPURIOUS) {
    HALT();

    while(1) {
      asm("hlt");
    }
  } else if(IdtCallVector == 241) {
    local_apic_id = APIC_LOCAL_GET_ID();
    dispatcher_cpu_entry = DispatcherGetCpu(local_apic_id);

    if((p = DispatcherFind(dispatcher_cpu_entry->current_pid)) == NULL) {
      HALT();
    }

    IdtCallSavedRax = KernelFunction(p, IdtCallSavedRax, IdtCallSavedRbx, IdtCallSavedRcx, IdtCallSavedRdx, IdtCallSavedRsi, IdtCallSavedRdi);
  } else if(IdtCallVector >= 0x30 && IdtCallVector <= 0x30 + 23) {
    // TODO
    INFO("I/O APIC interrupt %u", IdtCallVector - 0x30);

    if(IdtCallVector == IDT_IRQ_1) {
      KeyboardIrq();
    }
  } else {
    INFO("Unhandled interrupt");
  }

  dispatcher_cpu_entry = DispatcherGetCpu(APIC_LOCAL_GET_ID());
  if(dispatcher_cpu_entry == NULL) {
    HALT();
  } else if(dispatcher_cpu_entry->current_pid == 0) {
    // There is no process currently running on this CPU, so return to IdtWait()
    IdtWait();
    __builtin_unreachable();
  }
}

inline void IdtCreateGate(const uint8_t vector, void (*offset)(), uint16_t cs, uint8_t flags) {
  IdtGates[vector].cs = cs;
  IdtGates[vector].flags = (flags | IDT_GATE_INTGATE_DEFAULT);

  IdtGates[vector].offset_0_15 = (uint16_t) ((uint64_t) offset & 0xFFFF);
  IdtGates[vector].offset_16_31 = (uint16_t) ((uint64_t) offset >> 16);
  IdtGates[vector].offset_32_63 = ((uint64_t) offset >> 32);
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
  IdtCreateGate(IDT_IRQ_2, Idt50, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);
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

  // Software interrupt
  IdtCreateGate(241, Idt241, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_3);

  // Start interrupts on APs
  IdtCreateGate(242, Idt242, 0x08, IDT_GATE_INTGATE_PRESENT | IDT_GATE_INTGATE_PRIV_0);

  IdtLimit = (IDT_GATE_COUNT * sizeof(IdtGate_t)) - 1;
  IdtBase = (uint64_t) &IdtGates;
}
