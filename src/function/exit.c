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
 * End the process and return a value to the caller of the process.
 * @param  p            The process.
 * @param  return_value The return value.
 */
void KernelFunctionExit(DispatcherProcess_t *p, uint64_t return_value)
{
  p->pending_exit = 1;
  KernelFunctionSuspend(p, DISPATCHER_SUSPEND_EXIT, NULL);

  __builtin_unreachable();
}
