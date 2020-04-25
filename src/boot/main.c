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
#include <hypnoticos/devices/storage.h>
#include <hypnoticos/fs.h>

/*!
   \brief Called by Start.
   \param magic Magic value
   \param multiboot multiboot_info_t struct
*/
void Main(uint32_t magic, multiboot_info_t *multiboot) {
  VideoMemoryInit();
  puts(_HYPNOTICOS);
  puts("HypnoticOS  Copyright (C) 2019, 2020  jk30");
  puts("This program comes with ABSOLUTELY NO WARRANTY; for details see the LICENSE");
  puts("file in the source code. This is free software, and you are welcome to");
  puts("redistribute it under certain conditions; see the aforementioned file.");

  IdtInit();
  IdtSet();

  CpuChecks(CPU_BSP);

  // *** Free memory should not be used before this point ***

  MultibootCheck(magic, multiboot);

  INFO("Finish setting up paging");
  MemoryPagingInit();

  INFO("Init TSS AP entries table");
  if(!TssInitApEntries()) {
    HALT();
  }

  INFO("Dispatcher set up");
  if(!DispatcherInit()) {
    HALT();
  }

  INFO("Finding ACPI RSDP");
  AcpiFindRsdp(); // Needs access to BIOS Data Area

  INFO("BSP APIC");
  CpuApic(CPU_BSP); // Needs DispatcherInit() to have been called

  INFO("Parse ACPI tables");
  if(!AcpiParse()) { // This function finds the I/O APIC
    HALT();
  }

  INFO("Init I/O APIC");
  if(!ApicIoInit()) { // Needs AcpiParse() to have been called as AcpiParse() will find the I/O APIC
    HALT();
  }

  INFO("Keyboard");
  KeyboardInit();
  if(!KeyboardPresent) {
    printf("No PS/2 keyboard detected\n");
  }

  INFO("Storage: init");
  StorageInit();

  INFO("FS: init");
  FsInit();

  INFO("Load memory disk module");
  // Needs TSS structures to have been created
  if(!BootLoadModules()) {
    HALT();
  }

  INFO("PCI init");
  if(!PciInit()) {
    HALT();
  }

  INFO("BSP: APIC timer");
  ApicLocalSetUpTimer();

  printf("Starting...\n");

  asm("sti");

  while(1) {
    asm("hlt");
  }
}
