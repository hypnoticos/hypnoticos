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

#include <string.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/unimplemented.h> // TODO Remove once implemented
#include <hypnoticos/hypnoticos.h>

uint8_t ApicIoOkay = 0;
AcpiApicIo_t ApicIo;

inline uint32_t ApicIoRead(AcpiApicIo_t *io_apic, uint8_t r);
inline void ApicIoWrite(AcpiApicIo_t *io_apic, uint8_t r, uint32_t data);
void ApicIoIrq(uint8_t no, uint8_t vector);

uint8_t ApicIoAdd(AcpiApicIo_t *ptr) {
  if(ApicIoOkay != 0) {
    // TODO Support multiple I/O APICs
    WARNING();
    return 0;
  }

  ApicIoOkay = 1;
  memcpy(&ApicIo, ptr, sizeof(AcpiApicIo_t));
  return 1;
}

uint8_t ApicIoInit() {
  // TODO
  return 1;
}

void ApicIoMapIrqs() {
  uint32_t i;

  // TODO Max redirection entry
  for(i = 0; i <= 23; i++) {
    ApicIoIrq(i, 0x30 + i);
  }
}

#define APIC_IO_IOREGSEL          0x00
#define APIC_IO_IOWIN             0x10

inline uint32_t ApicIoRead(AcpiApicIo_t *io_apic, uint8_t r) {
  *((uint32_t *) ((uint64_t) io_apic->addr + APIC_IO_IOREGSEL)) = (uint32_t) r;
  return *((uint32_t *) ((uint64_t) io_apic->addr + APIC_IO_IOWIN));
}

inline void ApicIoWrite(AcpiApicIo_t *io_apic, uint8_t r, uint32_t data) {
  *((uint32_t *) ((uint64_t) io_apic->addr + APIC_IO_IOREGSEL)) = (uint32_t) r;
  *((uint32_t *) ((uint64_t) io_apic->addr + APIC_IO_IOWIN)) = data;
}

void ApicIoIrq(uint8_t no, uint8_t vector) {
  uint8_t local_apic_id;
  uint32_t value_low, value_high;

  // TODO Identify P6 family and Pentium processors - only retrieve bits 24 to 27
  local_apic_id = APIC_LOCAL_READ(ApicLocalBspBase, 0x20) & 0xFF000000;

  // TODO Determine whether the other flags are to be set
  value_low = vector;
  value_high = (local_apic_id << 24);

  ApicIoWrite(&ApicIo, 0x10 + (no * 2), value_low);
  ApicIoWrite(&ApicIo, 0x11 + (no * 2), value_high);
}
