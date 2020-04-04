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

uint64_t KernelFunctionWrite(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionSleep(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionRead(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionExit(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionNewPage(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionHeapAddr(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionHeapSize(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionRun(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionDirectoryGet(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionDirectoryDone(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionDirectoryEntry(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
void KernelFunctionSleep_SuspendTest(DispatcherProcess_t *p);
void KernelFunctionRead_SuspendTest(DispatcherProcess_t *p);

#endif

#endif
