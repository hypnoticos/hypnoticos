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

#ifndef HYPNOTICOS_FUNCTION_H
#define HYPNOTICOS_FUNCTION_H

#include <hypnoticos/dispatcher.h>
#include <stdint.h>

#define GET_PA(va)                    DispatcherProcessGetPa(p, va, 1);

uint64_t KernelFunction(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi);
void KernelFunctionSuspend(DispatcherProcess_t *p, uint32_t suspend, void *data);
void KernelFunctionSuspendTest(DispatcherProcess_t *p);
void KernelFunctionDone(DispatcherProcess_t *p, uint64_t rax);

#endif
