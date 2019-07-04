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
#include <stddef.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>

#define CALL_FUNCTION(f)                return f(p, rax, rbx, rcx, rdx, rsi, rdi)

uint64_t KernelFunction(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi) {
  switch(rdi) {
    case KERNEL_FUNCTION_WRITE:
    CALL_FUNCTION(KernelFunctionWrite);
    break;

    case KERNEL_FUNCTION_SLEEP:
    CALL_FUNCTION(KernelFunctionSleep);
    break;

    case KERNEL_FUNCTION_READ:
    CALL_FUNCTION(KernelFunctionRead);
    break;

    default:
    // TODO Terminate process
    WARNING();
    return 0;
  }
}

void KernelFunctionSuspend(DispatcherProcess_t *p, uint32_t suspend, void *data) {
  p->suspend = suspend;
  p->suspend_data = data;

  DispatcherSetUpNext(APIC_LOCAL_GET_ID());
}

void KernelFunctionSuspendTest(DispatcherProcess_t *p) {
  switch(p->suspend) {
    case DISPATCHER_SUSPEND_SLEEP:
    KernelFunctionSleep_SuspendTest(p);
    break;

    case DISPATCHER_SUSPEND_READ:
    KernelFunctionRead_SuspendTest(p);
    break;

    default:
    // TODO Terminate process?
    WARNING();
    break;
  }
}

void KernelFunctionDone(DispatcherProcess_t *p, uint64_t rax) {
  p->suspend = DISPATCHER_SUSPEND_NONE;
  p->suspend_data = NULL;
  p->save.rax = rax;
}
