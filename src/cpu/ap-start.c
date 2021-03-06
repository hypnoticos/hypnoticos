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
#include <stdlib.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>

#define STACK_SIZE          8192

void ApStart() {
  CpuChecks(CPU_AP);
  CpuApic(CPU_AP);
  IdtSet();
  asm("sti");
  ApInitDone = 1;

  while(1) {
    asm("hlt");
  }

  __builtin_unreachable();
}

void *ApStartNewStack() {
  void *stack;
  stack = malloc_align(STACK_SIZE, ALIGN_4KB);
  return (void *) ((uint64_t) stack + STACK_SIZE - 8);
}
