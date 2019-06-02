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

extern void DispatcherTest();

/*!
   \brief Called by Start.
   \param magic Magic value
   \param multiboot multiboot_info_t struct
*/
void Main(uint32_t magic, multiboot_info_t *multiboot) {
  VideoMemoryInit();
  puts(_HYPNOTICOS);

  IdtInit();
  IdtSet();
  CpuChecks(CPU_BSP);

  // *** Free memory should not be used before this point ***

  MultibootCheck(magic, multiboot);
  MemoryPagingInit();
  AcpiFindRsdp(); // Needs access to BIOS Data Area
  CpuApic(CPU_BSP);

  DispatcherInit();

  KeyboardInit();
  if(!KeyboardPresent) {
    printf("No PS/2 keyboard detected\n");
  }

  BootLoadModules();

  if(!PciInit()) {
    HALT();
  }

  printf("Starting...\n");
  asm("sti");
  ApicLocalSetUpTimer();

  while(1) {
    asm("hlt");
  }
}
