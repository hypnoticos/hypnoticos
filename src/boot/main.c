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

#include <stdint.h>
#include <stdio.h>
#include <hypnoticos/video-memory.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>
#include <multiboot.h>
#include <hypnoticos/boot.h>
#include <hypnoticos/devices.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/memory.h>

/*!
   \brief Called by Start.
   \param magic Magic value
   \param multiboot multiboot_info_t struct
*/
void Main(uint32_t magic, multiboot_info_t *multiboot) {
  DispatcherProcess_t *p;

  VideoMemoryInit();
  puts(_HYPNOTICOS);

  TssInit();
  IdtInit();
  AcpiFindRsdp(); // Needs access to BIOS Data Area (which may be overwritten when memory management starts)

  MultibootCheck(magic, multiboot);

  CpuChecks();

  KeyboardInit();
  if(!KeyboardPresent) {
    printf("No PS/2 keyboard detected\n");
  }

  MemoryPagingInit();

  if(!PciInit()) {
    HALT();
  }

  DispatcherInit();

  p = DispatcherProcessNew("first-process", (uint32_t) DispatcherFirstProcess, 0);
  DispatcherProcessRun(p);

  p = DispatcherProcessNew("another-process", (uint32_t) DispatcherAnotherProcess, 0);
  DispatcherProcessRun(p);

  p = DispatcherProcessNew("another-process-2", (uint32_t) 0xFF000000, 3);
  if(!DispatcherProcessMap(p, 0xFF000000, (uint32_t) DispatcherAnotherProcess2, PAGING_PRESENT | PAGING_USER)) {
    HALT();
  }
  DispatcherProcessRun(p);

  asm("sti");
  ApicLocalSetUpTimer();

  while(1) {
    asm("hlt");
  }
}
