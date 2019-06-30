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

#ifndef HYPNOTICOS_FUNCTION_CODES_H
#define HYPNOTICOS_FUNCTION_CODES_H

#define KERNEL_FUNCTION_WRITE                       0x1
#define KERNEL_FUNCTION_SLEEP                       0x2

#ifdef _HYPNOTICOS_KERNEL

#include <hypnoticos/dispatcher.h>

typedef struct _FunctionSleep_t FunctionSleep_t;
struct _FunctionSleep_t {
  uint64_t tick;
  uint8_t overflow;
};

uint64_t KernelFunctionWrite(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t KernelFunctionSleep(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
void KernelFunctionSleep_SuspendTest(DispatcherProcess_t *p);

#endif

#endif
