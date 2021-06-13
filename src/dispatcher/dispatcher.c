//
// HypnoticOS
// Copyright (C) 2019, 2020  jk30
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

#include <stddef.h>
#include <stdlib.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/function.h>

// TODO Save all registers

uint64_t DispatcherCycle = 0;
uint16_t last_pid = 0;
DispatcherProcess_t **DispatcherProcesses;

DispatcherProcess_t *DispatcherFind(uint16_t pid) {
  uint32_t i;

  for(i = 0; DispatcherProcesses[i] != NULL; i++) {
    if(DispatcherProcesses[i]->pid == pid) {
      return DispatcherProcesses[i];
    }
  }

  return NULL;
}

DispatcherCpu_t *DispatcherGetCpu(uint8_t cpu) {
  uint16_t i;

  for(i = 0; DispatcherCpus[i] != NULL; i++) {
    if(DispatcherCpus[i]->apic_id == cpu) {
      return DispatcherCpus[i];
    }
  }

  HALT();
  __builtin_unreachable();
}

void DispatcherSave(uint8_t apic_id) {
  DispatcherProcess_t *p;
  DispatcherCpu_t *dispatcher_cpu;

  dispatcher_cpu = DispatcherGetCpu(apic_id);
  if(dispatcher_cpu->current_pid != 0) {
    if((p = DispatcherFind(dispatcher_cpu->current_pid)) == NULL) {
      HALT();
    }

    // Do not save CR3
    p->save.rsp = IdtCallSavedRsp;
    p->save.rbp = IdtCallSavedRbp;
    p->save.rip = IdtCallSavedRip;

    p->save.rax = IdtCallSavedRax;
    p->save.rbx = IdtCallSavedRbx;
    p->save.rcx = IdtCallSavedRcx;
    p->save.rdx = IdtCallSavedRdx;
    p->save.rsi = IdtCallSavedRsi;
    p->save.rdi = IdtCallSavedRdi;

    p->save.r8 = IdtCallSavedR8;
    p->save.r9 = IdtCallSavedR9;
    p->save.r10 = IdtCallSavedR10;
    p->save.r11 = IdtCallSavedR11;
    p->save.r12 = IdtCallSavedR12;
    p->save.r13 = IdtCallSavedR13;
    p->save.r14 = IdtCallSavedR14;
    p->save.r15 = IdtCallSavedR15;

    p->save.rflags = IdtCallSavedRflags;

    p->lock = 0;

    // Reset I/O port bitmap
    for(uint64_t i = 0; i < p->io_count; i++) {
      uint32_t byte_offset = p->io[i] / 8;
      Tss.io_permission[byte_offset] = 0xFF;
    }

    dispatcher_cpu->current_pid = 0;
  }
}

#ifndef _HYPNOTICOS_TESTS
// This code should not be subject to tests on the host compiling the kernel.

void DispatcherSetUpNext(uint8_t apic_id) {
  DispatcherProcess_t *p;
  uint8_t no_processes_to_run;
  uint32_t byte_offset, bit_offset, bit_operation;
  DispatcherCpu_t *dispatcher_cpu;

  DispatcherSave(apic_id);

  dispatcher_cpu = DispatcherGetCpu(apic_id);

  // Find next process
  // Set this above restart because pending_exit code may jump to restart label
  // and the process_no will need to be preserved so as to not repeat
  // considering processes.
  uint64_t process_no = 0;
restart:
  no_processes_to_run = 1;
  for(p = NULL; DispatcherProcesses[process_no] != NULL; process_no++) {
    p = DispatcherProcesses[process_no];

    if(p->pending_exit) {
      DispatcherProcessDone(p);
      goto restart;
    }

    if(p->suspend != DISPATCHER_SUSPEND_NONE) {
      KernelFunctionSuspendTest(p);
    }

    if(p->run != 1 || p->lock != 0 || p->last_cycle == DispatcherCycle || p->suspend != DISPATCHER_SUSPEND_NONE) {
      if(p->last_cycle == DispatcherCycle) {
        no_processes_to_run = 0;
      }
      p = NULL;
      continue;
    } else {
      break;
    }
  }

  if(p == NULL) {
    if(no_processes_to_run) {
      dispatcher_cpu->current_pid = 0;
      // Release lock and wait for another process
      IdtWait();
      __builtin_unreachable();
    }
    DispatcherCycle++; // TODO Handle overflow
    process_no = 0;
    goto restart;
  }

  p->last_cycle = DispatcherCycle;
  p->lock = 1;
  dispatcher_cpu->current_pid = p->pid;

  // Restore registers
  IdtCallSavedCr3 = p->save.cr3;

  IdtCallSavedRip = p->save.rip;

  IdtCallSavedRsp = p->save.rsp;
  IdtCallSavedRbp = p->save.rbp;

  IdtCallSavedRax = p->save.rax;
  IdtCallSavedRbx = p->save.rbx;
  IdtCallSavedRcx = p->save.rcx;
  IdtCallSavedRdx = p->save.rdx;
  IdtCallSavedRsi = p->save.rsi;
  IdtCallSavedRdi = p->save.rdi;
  IdtCallSavedR8 = p->save.r8;
  IdtCallSavedR9 = p->save.r9;
  IdtCallSavedR10 = p->save.r10;
  IdtCallSavedR11 = p->save.r11;
  IdtCallSavedR12 = p->save.r12;
  IdtCallSavedR13 = p->save.r13;
  IdtCallSavedR14 = p->save.r14;
  IdtCallSavedR15 = p->save.r15;

  // Set up I/O port bitmap
  for(uint64_t i = 0; i < p->io_count; i++) {
    byte_offset = p->io[i] / 8;
    bit_offset = p->io[i] % 8;
    bit_operation = 0x1 << bit_offset;
    Tss.io_permission[byte_offset] = Tss.io_permission[byte_offset] ^ bit_operation;
  }
}

#endif

void DispatcherProcessAddIo(DispatcherProcess_t *p, uint16_t port) {
  p->io_count++;
  p->io = realloc(p->io, sizeof(uint16_t) * p->io_count);
  p->io[p->io_count - 1] = port;
}

void DispatcherProcessRun(DispatcherProcess_t *p) {
  p->run = 1;
}

void DispatcherProcessSetRip(DispatcherProcess_t *p, uint64_t rip) {
  p->save.rip = rip;
}
