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

#include <hypnoticos/dispatcher/format-elf.h>

#define DISPATCHER_FORMAT_ELF             0x01

#define DISPATCHER_DETECT_FORMAT_NOT_DETECTED         0
#define DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED 1
#define DISPATCHER_DETECT_FORMAT_DETECTED             2
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
  void *stack;
  DispatcherProcessSave_t save;
  uint8_t run;

  char *data; /*!< Optional parameter - a copy of the original file. Set to NULL if not present. */
  uint32_t size; /*!< Data size */
  uint32_t format;
  void *format_header;

  void **alloc; /*!< Memory that must be free'd upon process termination */
};

uint8_t DispatcherInit();
extern void DispatcherInterrupt();
void *DispatcherProcessAllocatePage(DispatcherProcess_t *p, uint32_t va, uint32_t flags);
uint8_t DispatcherProcessLoadAt(DispatcherProcess_t *p, uint32_t va, char *data, uint32_t file_size, uint32_t memory_size, uint32_t flags);
uint8_t DispatcherProcessMap(DispatcherProcess_t *p, uint32_t va, uint32_t pa, uint32_t flags);
DispatcherProcess_t *DispatcherProcessNew(char *name);
DispatcherProcess_t *DispatcherProcessNewFromFormat(char *name, char *data, uint32_t size);
void DispatcherProcessRun(DispatcherProcess_t *p);
void DispatcherProcessSetEip(DispatcherProcess_t *p, uint32_t eip);
void DispatcherSetUpNext();
uint8_t DispatcherProcessSetUpStack(DispatcherProcess_t *p, uint32_t size);

#endif
