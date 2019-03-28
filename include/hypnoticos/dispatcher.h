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
struct _DispatcherProcessSave_t { // NOTE: The order and layout of this structure is relied upon elsewhere
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t esp;
  uint32_t ebp;
} __attribute__((packed));
struct _DispatcherProcess_t {
  uint16_t pid;
  char *name;
  void *stack;
  DispatcherProcessSave_t save;
};

void DispatcherFirstProcess();
uint8_t DispatcherInit();
extern void DispatcherInterrupt();
uint8_t DispatcherNew(char *name, void *addr);

void DispatcherAnotherProcess();

#endif
