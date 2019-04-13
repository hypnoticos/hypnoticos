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

#ifndef __HYPNOTICOS_DISPATCHER_H
#define __HYPNOTICOS_DISPATCHER_H

#include <stdint.h>

typedef struct _DispatcherProcess_t DispatcherProcess_t;
typedef struct _DispatcherProcessSave_t DispatcherProcessSave_t;
struct _DispatcherProcessSave_t {
  uint32_t esp;
  uint32_t ebp;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;

  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
};
struct _DispatcherProcess_t {
  uint16_t pid;
  char *name;
  uint8_t privilege_level;
  void *stack;
  DispatcherProcessSave_t save;
  uint8_t run;
};

uint8_t DispatcherInit();
extern void DispatcherInterrupt();
uint8_t DispatcherProcessMap(DispatcherProcess_t *p, uint32_t va, uint32_t pa, uint32_t flags);
DispatcherProcess_t *DispatcherProcessNew(char *name, uint32_t eip, uint8_t privilege_level);
void DispatcherProcessRun(DispatcherProcess_t *p);
void DispatcherSetUpNext();

void DispatcherFirstProcess();
void DispatcherAnotherProcess();
void DispatcherAnotherProcess2();

#endif
