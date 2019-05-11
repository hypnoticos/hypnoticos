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

#include <hypnoticos/devices.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>
#include <stdlib.h>
#include <stddef.h>

#define PCI_ADDRESS(bus, device, function, offset_multiple)     ((0x1 << 31) | ((bus) << 16) | (((device) & 0x1F) << 11) | (((function) & 0x7) << 8) | ((offset_multiple) * 4))

PciCache_t **PciCache = NULL;

uint32_t inline PciRead(uint32_t address) {
  IoPort32Out(0xCF8, address);
  return IoPort32In(0xCFC);
}

void inline PciWrite(uint32_t address, uint32_t data) {
  IoPort32Out(0xCF8, address);
  IoPort32Out(0xCFC, data);
}

uint8_t PciInit() {
  uint32_t bus, device, function;
  uint32_t buffer;
  uint8_t i;
  uint32_t count = 0;

  // TODO Search all bridges (enumerate further)
  PciCache = malloc(sizeof(PciCache_t *));
  PciCache[0] = NULL;
  for(bus = 0; bus <= 255; bus++) {
    for(device = 0; device <= 32; device++) {
      for(function = 0; function <= 8; function++) {
        buffer = PciRead(PCI_ADDRESS(bus, device, function, 0));
        if(buffer != 0xFFFFFFFF) {
          // TODO Modular drivers
          buffer = PciRead(PCI_ADDRESS(bus, device, function, 3));
          if((((buffer & 0xFF0000) >> 16) & 0x7F) == 0x00) {
            PciCache = realloc(PciCache, sizeof(PciCache_t *) * (count + 2));
            PciCache[count] = malloc(sizeof(PciCache_t));
            PciCache[count + 1] = NULL;

            for(i = 0; i < 16; i++) {
              *((uint32_t *) ((uint32_t) &PciCache[count]->hdr0 + (i * 4))) = PciRead(PCI_ADDRESS(bus, device, function, i));
            }

            PciCache[count]->bus = bus;
            PciCache[count]->device = device;
            PciCache[count]->function = function;

            count++;
          } else {
            // TODO
            WARNING();
          }
        }
      }
    }
  }

  return 1;
}
