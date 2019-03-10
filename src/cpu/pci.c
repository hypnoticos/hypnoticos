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

#include <hypnoticos/cpu.h>

uint32_t PciRead(uint32_t address) {
  IoPort32Out(0xCF8, address);
  return IoPort32In(0xCFC);
}

uint8_t PciInit() {
  uint32_t bus, device, function;
  uint32_t buffer;
  uint32_t device_id, vendor_id;

  (void) device_id;
  (void) vendor_id;

  for(bus = 0; bus <= 255; bus++) {
    for(device = 0; device <= 32; device++) {
      for(function = 0; function <= 8; function++) {
        buffer = PciRead((0x1 << 31) | (bus << 16) | ((device & 0x1F) << 11) | ((function & 0x7) << 8));
        if(buffer != 0xFFFFFFFF) {
          device_id = buffer >> 16;
          vendor_id = buffer & 0xFFFF;

          // TODO Modular drivers
        }
      }
    }
  }

  return 1;
}
