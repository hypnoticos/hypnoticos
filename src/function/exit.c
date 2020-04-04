//
// HypnoticOS
// Copyright (C) 2020  jk30
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

#include <stddef.h>
#include <hypnoticos/function.h>
#include <hypnoticos/dispatcher.h>

/**
 * Causes a process to be ended.
 * @param  p   The process.
 * @param  rax The return value.
 * @param  rbx Not used.
 * @param  rcx Not used.
 * @param  rdx Not used.
 * @param  rsi Not used.
 * @param  rdi Not used.
 * @return     Never returns.
 */
uint64_t KernelFunctionExit(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi)
{
  p->pending_exit = 1;
  KernelFunctionSuspend(p, DISPATCHER_SUSPEND_EXIT, NULL);

  __builtin_unreachable();
}
