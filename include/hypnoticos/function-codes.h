//
// HypnoticOS
// Copyright (C) 2019, 2020, 2024  jk30
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

#ifndef HYPNOTICOS_FUNCTION_CODES_H
#define HYPNOTICOS_FUNCTION_CODES_H

#define KERNEL_FUNCTION_WRITE                       0x1
#define KERNEL_FUNCTION_SLEEP                       0x2
#define KERNEL_FUNCTION_READ                        0x3
#define KERNEL_FUNCTION_EXIT                        0x4
#define KERNEL_FUNCTION_NEW_PAGE                    0x5
#define KERNEL_FUNCTION_HEAP_ADDR                   0x6
#define KERNEL_FUNCTION_HEAP_SIZE                   0x7
#define KERNEL_FUNCTION_RUN                         0x8
#define KERNEL_FUNCTION_DIRECTORY_GET               0x9
#define KERNEL_FUNCTION_DIRECTORY_DONE              0xA
#define KERNEL_FUNCTION_DIRECTORY_ENTRY             0xB
#define KERNEL_FUNCTION_GETCWD                      0xC
#define KERNEL_FUNCTION_CHDIR                       0xD

#ifdef _HYPNOTICOS_KERNEL

#include <hypnoticos/dispatcher.h>

typedef struct _FunctionSleep_t FunctionSleep_t;
struct _FunctionSleep_t {
  uint64_t tick;
  uint8_t overflow;
};

typedef struct _FunctionRead_t FunctionRead_t;
struct _FunctionRead_t {
  int fd;
  char *buffer;
  size_t count;
  size_t offset;
};

uint64_t KernelFunctionWrite(DispatcherProcess_t *p, uint64_t fd, uint64_t buffer_addr, uint64_t count);
void KernelFunctionSleep(DispatcherProcess_t *p, uint64_t ticks);
void KernelFunctionRead(DispatcherProcess_t *p, uint64_t fd, uint64_t buffer_addr, uint64_t count);
void KernelFunctionExit(DispatcherProcess_t *p, uint64_t return_value);
uint64_t KernelFunctionNewPage(DispatcherProcess_t *p, uint64_t count);
uint64_t KernelFunctionHeapAddr(DispatcherProcess_t *p);
uint64_t KernelFunctionHeapSize(DispatcherProcess_t *p);
uint64_t KernelFunctionRun(DispatcherProcess_t *p, uint64_t path, uint64_t argv, uint64_t argc);
uint64_t KernelFunctionDirectoryGet(DispatcherProcess_t *p, uint64_t path);
uint64_t KernelFunctionDirectoryDone(DispatcherProcess_t *p, uint64_t lock_entry_id);
uint64_t KernelFunctionDirectoryEntry(DispatcherProcess_t *p, uint64_t lock_entry_id, uint64_t offset, uint64_t index_addr);
uint64_t KernelFunctionGetCurrentWorkingDirectory(DispatcherProcess_t *p, uint64_t buffer, uint64_t size);
uint64_t KernelFunctionChangeCurrentWorkingDirectory(DispatcherProcess_t *p, uint64_t new_dir);
void KernelFunctionSleep_SuspendTest(DispatcherProcess_t *p);
void KernelFunctionRead_SuspendTest(DispatcherProcess_t *p);

#endif

#endif
