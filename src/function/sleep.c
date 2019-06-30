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

#include <stdlib.h>
#include <unistd.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>

// TODO Implement this fully.
// NOTE: For now, this function does not wait the requested number of seconds, but instead pauses for a small period of time based upon the number of seconds requested to wait
uint64_t KernelFunctionSleep(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi) {
  uint64_t ticks_current, ticks_wait;
  FunctionSleep_t *data;

  if(p->suspend != DISPATCHER_SUSPEND_NONE) {
    // TODO End process?
    INFO("Tried to suspend a process which is already suspended");
    HALT();
  }

  ticks_current = IdtTicks;
  ticks_wait = rax * 10000;

  // TODO Lock struct

  data = malloc(sizeof(FunctionSleep_t));

  data->tick = ticks_current + ticks_wait;
  data->overflow = ((ticks_current + ticks_wait) < ticks_current) ? 1 : 0;

  KernelFunctionSuspend(p, DISPATCHER_SUSPEND_SLEEP, data);
}

void KernelFunctionSleep_SuspendTest(DispatcherProcess_t *p) {
  FunctionSleep_t *data;
  uint64_t ticks_current;

  ticks_current = IdtTicks;

  // TODO Lock struct

  data = p->suspend_data;
  if(data->overflow) {
    // TODO
  } else {
    if(ticks_current >= data->tick) {
      // Done
      free(p->suspend_data);
      p->suspend = DISPATCHER_SUSPEND_NONE;
    }
  }

  // TODO Return value
}
