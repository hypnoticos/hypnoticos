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
uint32_t PciCacheCount = 0;

uint8_t PciIterateBus(uint8_t bus);
uint8_t PciParseFunction(uint8_t bus, uint8_t device, uint8_t function);

uint32_t inline PciRead(uint32_t address) {
  IoPort32Out(0xCF8, address);
  return IoPort32In(0xCFC);
}

void inline PciWrite(uint32_t address, uint32_t data) {
  IoPort32Out(0xCF8, address);
  IoPort32Out(0xCFC, data);
}

uint8_t PciInit() {
  uint32_t buffer;

  // PCI host controller is usually at 0, 0, 0
  // But, there may be multiple controllers or it may not actually be at 0, 0, 0
  // TODO Use AML to find out where the host controller(s) are

  PciCache = malloc(sizeof(PciCache_t *));
  PciCache[0] = NULL;

  buffer = PciRead(PCI_ADDRESS(0, 0, 0, 0));
  if(buffer == 0xFFFFFFFF) {
    WARNING();
    return 0;
  }

  buffer = PciRead(PCI_ADDRESS(0, 0, 0, 3));
  if((((buffer & 0xFF0000) >> 16) & 0x7F) != 0x00) {
    // TODO Support other header types
    WARNING();
    return 0;
  }

  return PciIterateBus(0);
}

uint8_t PciIterateBus(uint8_t bus) {
  uint8_t device;
  uint8_t header_type;
  uint32_t buffer;

  // Iterate through each device on the bus
  for(device = 0; device < 32; device++) {
    buffer = PciRead(PCI_ADDRESS(bus, device, 0, 0));
    if(buffer != 0xFFFFFFFF) {
      // TODO Modular drivers
      buffer = PciRead(PCI_ADDRESS(bus, device, 0, 3));
      header_type = ((buffer & 0xFF0000) >> 16);
      if((header_type & 0x7F) == 0x00) {
        if((header_type & 0x80) == 0x80) {
          // Multiple functions
          for(uint8_t i = 0; i < 8; i++) {
            if(!PciParseFunction(bus, device, i)) {
              WARNING();
              return 0;
            }
          }
        } else {
          // One function
          if(!PciParseFunction(bus, device, 0)) {
            WARNING();
            return 0;
          }
        }
      } else {
        // TODO
        WARNING();
      }
    }
  }

  return 1;
}

uint8_t PciParseFunction(uint8_t bus, uint8_t device, uint8_t function) {
  PciCache = realloc(PciCache, sizeof(PciCache_t *) * (PciCacheCount + 2));
  PciCache[PciCacheCount] = malloc(sizeof(PciCache_t));
  PciCache[PciCacheCount + 1] = NULL;

  for(uint8_t i = 0; i < 16; i++) {
    *((uint32_t *) ((uint64_t) &PciCache[PciCacheCount]->hdr0 + (i * 4))) = PciRead(PCI_ADDRESS(bus, device, function, i));

    // Check first entry
    if(i == 0 && *((uint32_t *) ((uint64_t) &PciCache[PciCacheCount]->hdr0 + (i * 4))) == 0xFFFFFFFF) {
      return 1;
    }
  }

  PciCache[PciCacheCount]->bus = bus;
  PciCache[PciCacheCount]->device = device;
  PciCache[PciCacheCount]->function = function;

  INFO("PciCache[%u] bus=0x%X device=0x%X function=0x%X class=0x%X subclass=0x%X", PciCacheCount, bus, device, function, PciCache[PciCacheCount]->hdr0.hdr.class, PciCache[PciCacheCount]->hdr0.hdr.subclass);
  INFO("PciCache[%u] device_id=0x%X vendor_id=0x%X", PciCacheCount, PciCache[PciCacheCount]->hdr0.hdr.device_id, PciCache[PciCacheCount]->hdr0.hdr.vendor_id);
  for(uint8_t i = 0; i < PCI_MAX_BARS; i++) {
    if(PciCache[PciCacheCount]->hdr0.bar[i] == 0) {
      continue;
    }

    uint32_t bar = PciCache[PciCacheCount]->hdr0.bar[i];
    if((bar & 0x1) == 0x1) {
      if(bar > 0xFFFF) {
        WARNING();
        return 0;
      }
      INFO("PciCache[%u] BAR%u, I/O port = 0x%X", PciCacheCount, i, bar & 0xFFFFFFFC);
    } else {
      if(((bar >> 1) & 0x3) == 0x00) { // TODO Ensure paging can access this memory, and determine the size of the memory used - ensure that every page is not recorded as allocatable memory
        INFO("PciCache[%u] BAR%u, memory, 32-bit addr = 0x%X", PciCacheCount, i, bar & 0xFFFFFFF0);
      } else if(((bar >> 1) & 0x3) == 0x02) {
        if(i + 1 == PCI_MAX_BARS) {
          WARNING();
          return 0;
        }
        uint32_t bar_next = PciCache[PciCacheCount]->hdr0.bar[i + 1];
        INFO("PciCache[%u] BAR%u, memory, 64-bit addr = 0x%X%X", PciCacheCount, i, bar & 0xFFFFFFF0, bar_next & 0xFFFFFFF0);
        i++;
      }
    }
  }

  PciCacheCount++;

  return 1;
}
