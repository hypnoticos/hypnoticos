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
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/memory.h>

// TODO Save all registers

#define DISPATCHER_PROCESS_STACK_SIZE       8192
#if (DISPATCHER_PROCESS_STACK_SIZE % 4096) != 0
#error Check stack size
#endif

uint16_t DispatcherCurrentPid = 0;
uint16_t last_pid = 0;
DispatcherProcess_t **DispatcherProcesses;

inline DispatcherProcess_t *DispatcherFind(uint16_t pid);

inline DispatcherProcess_t *DispatcherFind(uint16_t pid) {
  uint32_t i;

  for(i = 0; DispatcherProcesses[i] != NULL; i++) {
    if(DispatcherProcesses[i]->pid == pid) {
      return DispatcherProcesses[i];
    }
  }

  return NULL;
}

void DispatcherSetUpNext() {
  DispatcherProcess_t *p;
  uint16_t next_pid;

  if(DispatcherCurrentPid != 0) {
    if((p = DispatcherFind(DispatcherCurrentPid)) == NULL) {
      HALT();
    }

    // Do not save CR3
    p->save.esp = IdtCallSavedEsp;
    p->save.ebp = IdtCallSavedEbp;
    p->save.eip = IdtCallSavedEip;

    p->save.eax = IdtCallSavedEax;
    p->save.ebx = IdtCallSavedEbx;
    p->save.ecx = IdtCallSavedEcx;
    p->save.edx = IdtCallSavedEdx;
    p->save.esi = IdtCallSavedEsi;
    p->save.edi = IdtCallSavedEdi;
  }

  // Find next process
  next_pid = DispatcherCurrentPid + 1;
  while(1) {
    if((p = DispatcherFind(next_pid)) != NULL && p->run == 1) {
      break;
    }

    if(next_pid == 0xFFFF) {
      next_pid = 1;
    } else {
      next_pid++;
    }
  }

  DispatcherCurrentPid = next_pid;

  printf("pid=%u. ", p->pid);

  IdtCallCurrentPrivilegeLevel = p->privilege_level;

  // Restore registers
  IdtCallSavedCr3 = p->privilege_level == 3 ? p->save.cr3 : (uint32_t) MemoryPD;

  IdtCallSavedEip = p->save.eip;

  IdtCallSavedEsp = p->save.esp;
  IdtCallSavedEbp = p->save.ebp;

  IdtCallSavedEax = p->save.eax;
  IdtCallSavedEbx = p->save.ebx;
  IdtCallSavedEcx = p->save.ecx;
  IdtCallSavedEdx = p->save.edx;
  IdtCallSavedEsi = p->save.esi;
  IdtCallSavedEdi = p->save.edi;
}

uint8_t DispatcherInit() {
  DispatcherProcesses = malloc(sizeof(DispatcherProcess_t *));
  DispatcherProcesses[0] = NULL;

  return 1;
}

DispatcherProcess_t *DispatcherProcessNew(char *name, uint32_t eip, uint8_t privilege_level) {
  DispatcherProcess_t *p;
  uint32_t i;

  p = malloc(sizeof(DispatcherProcess_t));
  memset(&p->save, 0, sizeof(DispatcherProcessSave_t));

  while(1) { // TODO Check if max processes reached
    if(last_pid == 0xFFFF) {
      last_pid = 1;
    } else {
      last_pid++;
    }

    if(DispatcherFind(last_pid) == NULL) {
      p->pid = last_pid;
      break;
    }
  }

  p->stack = (void *) malloc_align(DISPATCHER_PROCESS_STACK_SIZE, ALIGN_4KB);
  memset(p->stack, 0, DISPATCHER_PROCESS_STACK_SIZE);

  p->privilege_level = privilege_level;
  if(privilege_level == 0) {
    p->save.cr3 = 0;
  } else {
    p->save.cr3 = (uint32_t) MemoryPagingNewPD();

    // Note: This relies upon DISPATCHER_PROCESS_STACK_SIZE being divisible by 4096
    for(i = 0; i < DISPATCHER_PROCESS_STACK_SIZE; i += 4096) {
      if(!DispatcherProcessMap(p, 0xF0000000 + i, (uint32_t) p->stack + i, PAGING_PRESENT | PAGING_RW | PAGING_USER)) {
        // TODO Clean up
        return NULL;
      }
    }
  }

  p->save.ebp = (privilege_level == 3 ? 0xF0000000 : (uint32_t) p->stack) + DISPATCHER_PROCESS_STACK_SIZE - 1;
  p->save.esp = p->save.ebp;

  p->run = 0;

  p->name = malloc(strlen(name) + 1);
  strcpy(p->name, name);

  p->save.eip = eip;

  for(i = 0; DispatcherProcesses[i] != NULL; i++);

  DispatcherProcesses = realloc(DispatcherProcesses, sizeof(DispatcherProcess_t *) * (i + 2));
  DispatcherProcesses[i] = p;
  DispatcherProcesses[i + 1] = NULL;

  return p;
}

void DispatcherProcessRun(DispatcherProcess_t *p) {
  p->run = 1;
}

uint8_t DispatcherProcessMap(DispatcherProcess_t *p, uint32_t va, uint32_t pa, uint32_t flags) {
  if(p->privilege_level != 3) {
    return 0;
  } else {
    return MemoryPagingSetPage((uint32_t *) p->save.cr3, va, pa, flags);
  }
}
