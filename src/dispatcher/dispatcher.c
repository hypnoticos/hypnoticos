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

// TODO Save all registers

#define DISPATCHER_PROCESS_STACK_SIZE       8192

uint16_t current_pid = 0;
uint16_t last_pid = 0;

DispatcherProcess_t **DispatcherProcesses;

inline DispatcherProcess_t *DispatcherFind(uint16_t pid);

inline DispatcherProcess_t *DispatcherFind(uint16_t pid) {
  uint32_t i;

  for(i = 0; DispatcherProcesses[i] != NULL && i < 100; i++) {
    if(DispatcherProcesses[i]->pid == pid) {
      return DispatcherProcesses[i];
    }
  }

  return NULL;
}

uint32_t DispatcherPrepareForNext(uint32_t DoSave, DispatcherProcessSave_t save) {
  DispatcherProcess_t *p;
  uint16_t next_pid;

  if(DoSave) {
    // Save registers
    if((p = DispatcherFind(current_pid)) == NULL) {
      HALT();
    }
    memcpy(&p->save, &save, sizeof(DispatcherProcessSave_t));
  }

  // Find next process
  next_pid = current_pid + 1;
  while(1) {
    if((p = DispatcherFind(next_pid)) != NULL) {
      break;
    }

    next_pid++;
  }

  current_pid = next_pid;

  printf("pid=%u(%u). ", p->pid, current_pid);

  ApicLocalEoi();

  return (uint32_t) &p->save;
}

uint8_t DispatcherInit() {
  DispatcherProcesses = malloc(sizeof(DispatcherProcess_t *));
  DispatcherProcesses[0] = NULL;

  return 1;
}

uint8_t DispatcherNew(char *name, void *addr) {
  DispatcherProcess_t *p;
  uint32_t i;

  p = malloc(sizeof(DispatcherProcess_t));

  while(1) { // TODO Check if max processes reached
    last_pid++;

    if(DispatcherFind(last_pid) == NULL) {
      p->pid = last_pid;
      break;
    }
  }

  p->name = malloc(strlen(name) + 1);
  strcpy(p->name, name);

  memset(&p->save, 0, sizeof(DispatcherProcessSave_t));

  p->stack = malloc(DISPATCHER_PROCESS_STACK_SIZE + 3); // +3 ensures that there will be 8KB of 4 byte aligned space (TODO align correctly)
  memset(p->stack, 0, DISPATCHER_PROCESS_STACK_SIZE + 3);
  p->save.ebp = ((uint32_t) p->stack) + DISPATCHER_PROCESS_STACK_SIZE + 3 - ((((uint32_t) p->stack) + DISPATCHER_PROCESS_STACK_SIZE + 3) % 4);
  p->save.esp = p->save.ebp - 12;

  // Set up the stack
  *((uint32_t *)((uint32_t) p->save.ebp - 4)) = (uint32_t) EflagsGet() | 0x200; // EFLAGS
  *((uint32_t *)((uint32_t) p->save.ebp - 8)) = (uint32_t) 0x08;         // CS
  *((uint32_t *)((uint32_t) p->save.ebp - 12)) = (uint32_t) addr;        // EIP

  for(i = 0; DispatcherProcesses[i] != NULL; i++);

  DispatcherProcesses = realloc(DispatcherProcesses, sizeof(DispatcherProcess_t *) * (i + 2));
  DispatcherProcesses[i] = p;
  DispatcherProcesses[i + 1] = NULL;

  return 1;
}
