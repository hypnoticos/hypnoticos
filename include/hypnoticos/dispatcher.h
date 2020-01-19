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

#ifndef HYPNOTICOS_DISPATCHER_H
#define HYPNOTICOS_DISPATCHER_H

typedef struct _DispatcherProcess_t DispatcherProcess_t;
typedef struct _DispatcherProcessVa_t DispatcherProcessVa_t;
typedef struct _DispatcherProcessSave_t DispatcherProcessSave_t;

#include <hypnoticos/dispatcher/format-elf.h>
#include <sys/types.h>
#include <stdint.h>

#define DISPATCHER_FORMAT_ELF                         0x01

#define DISPATCHER_DETECT_FORMAT_NOT_DETECTED         0
#define DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED 1
#define DISPATCHER_DETECT_FORMAT_DETECTED             2

#define DISPATCHER_SUSPEND_NONE                       0x00
#define DISPATCHER_SUSPEND_SLEEP                      0x01
#define DISPATCHER_SUSPEND_READ                       0x02

struct _DispatcherProcessSave_t {
  uint64_t rsp;
  uint64_t rbp;
  uint64_t cr3;
  uint64_t rip;
  uint64_t rflags;

  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
};

struct _DispatcherProcessVa_t {
  uint64_t va;
  void *pa;
  uint8_t ignore; /*!< 0 = Kernel functions will not ignore this virtual address range. 1 = Kernel funtions will ignore this virtual address range. */
};

struct _DispatcherProcess_t {
  uint16_t pid;
  char *name;
  void *stack;
  uint64_t heap_addr;
  uint64_t heap_size;
  DispatcherProcessSave_t save;
  uint8_t run;
  uint64_t last_cycle;

  char *data; /*!< Optional parameter - a copy of the original file. Set to NULL if not present. */
  uint64_t size; /*!< Data size */
  uint32_t format;
  void *format_header;

  void **alloc; /*!< Memory that must be free'd upon process termination */

  DispatcherProcessVa_t **va;

  uint16_t *io;
  uint32_t io_count;

  uint8_t lock;

  uint32_t suspend;
  void *suspend_data;
};

typedef struct _DispatcherCpu_t DispatcherCpu_t;
struct _DispatcherCpu_t {
  uint8_t apic_id;
  uint16_t current_pid;
};

extern DispatcherProcess_t **DispatcherProcesses;
extern DispatcherCpu_t **DispatcherCpus;

DispatcherProcess_t *DispatcherFind(uint16_t pid);
DispatcherCpu_t *DispatcherGetCpu(uint8_t cpu);
uint8_t DispatcherInit();
uint8_t DispatcherInitAddCpu(uint8_t apic_id);
extern void DispatcherInterrupt();
void DispatcherProcessAddIo(DispatcherProcess_t *p, uint16_t port);
void *DispatcherProcessAllocatePage(DispatcherProcess_t *p, uint64_t va, uint8_t kernel_function_ignore, uint32_t flags);
void *DispatcherProcessGetPa(DispatcherProcess_t *p, uint64_t va, uint8_t ignore);
uint8_t DispatcherProcessLoadAt(DispatcherProcess_t *p, uint64_t va, char *data, uint64_t file_size, uint64_t memory_size, uint32_t flags);
uint8_t DispatcherProcessMap(DispatcherProcess_t *p, uint64_t va, uint64_t pa, uint8_t kernel_function_ignore, uint32_t flags);
DispatcherProcess_t *DispatcherProcessNew(char *name);
DispatcherProcess_t *DispatcherProcessNewFromFormat(char *name, char *data, uint64_t size);
void DispatcherProcessRun(DispatcherProcess_t *p);
void DispatcherProcessSetRip(DispatcherProcess_t *p, uint64_t rip);
void DispatcherSave(uint8_t apic_id);
void DispatcherSetUpNext(uint8_t apic_id);
uint8_t DispatcherProcessSetUpStack(DispatcherProcess_t *p, uint64_t size);

#endif
