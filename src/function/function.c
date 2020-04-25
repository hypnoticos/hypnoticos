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

#include <stdio.h>
#include <stddef.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>

/**
 * Call a kernel function. This function will be called as a result of a
 * process's interrupt.
 * @param  p   The process struct for the process calling this function.
 * @param  rax Parameter 1.
 * @param  rbx Parameter 2.
 * @param  rcx Parameter 3.
 * @param  rdx Parameter 4.
 * @param  rsi Parameter 5.
 * @param  rdi The code for the kernel function to be called.
 * @return     The return value of the kernel function.
 */
uint64_t KernelFunction(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi)
{
  switch(rdi) {
    case KERNEL_FUNCTION_WRITE:
    return KernelFunctionWrite(p, rax, rbx, rcx);
    break;

    case KERNEL_FUNCTION_SLEEP:
    KernelFunctionSleep(p, rax);
    break;

    case KERNEL_FUNCTION_READ:
    KernelFunctionRead(p, rax, rbx, rcx);
    break;

    case KERNEL_FUNCTION_EXIT:
    KernelFunctionExit(p, rax);
    break;

    case KERNEL_FUNCTION_NEW_PAGE:
    return KernelFunctionNewPage(p, rax);
    break;

    case KERNEL_FUNCTION_HEAP_ADDR:
    return KernelFunctionHeapAddr(p);
    break;

    case KERNEL_FUNCTION_HEAP_SIZE:
    return KernelFunctionHeapSize(p);
    break;

    case KERNEL_FUNCTION_RUN:
    return KernelFunctionRun(p, rax);
    break;

    case KERNEL_FUNCTION_DIRECTORY_GET:
    return KernelFunctionDirectoryGet(p, rax);
    break;

    case KERNEL_FUNCTION_DIRECTORY_DONE:
    return KernelFunctionDirectoryDone(p, rax);
    break;

    case KERNEL_FUNCTION_DIRECTORY_ENTRY:
    return KernelFunctionDirectoryEntry(p, rax, rbx, rcx);
    break;

    default:
    // TODO Terminate process
    WARNING();
    return 0;
  }
}

/**
 * Certain kernel functions can suspend the process.
 * @param p       The process struct for the process to be suspended.
 * @param suspend The suspension code.
 * @param data    The data for the suspension.
 */
void KernelFunctionSuspend(DispatcherProcess_t *p, uint32_t suspend, void *data)
{
  p->suspend = suspend;
  p->suspend_data = data;

  DispatcherSave(APIC_LOCAL_GET_ID());
  IdtWait();

  __builtin_unreachable();
}

/**
 * This function is used by the dispatcher to test whether the suspension can be
 * lifted.
 * @param p The process struct for which the suspension will be tested.
 */
void KernelFunctionSuspendTest(DispatcherProcess_t *p)
{
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

/**
 * This function lifts the suspension.
 * @param p   The process struct for which the suspension will be lifted.
 * @param rax The return value from the kernel function call.
 */
void KernelFunctionDone(DispatcherProcess_t *p, uint64_t rax)
{
  p->suspend = DISPATCHER_SUSPEND_NONE;
  p->suspend_data = NULL;
  p->save.rax = rax;
}
