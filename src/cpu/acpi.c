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
#include <string.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/hypnoticos.h>

void *BiosEbda = NULL;

inline uint8_t AcpiTestRsdp(void *ptr);

typedef struct _AcpiApic_t AcpiApic_t;
struct _AcpiApic_t {
  AcpiTableHeader_t hdr;
  uint32_t local_int_ctrlr_addr;
  uint32_t flags;
  // Entries are here
} __attribute__((packed));

AcpiRsdp_t *AcpiRsdp = NULL;
AcpiRsdt_t *AcpiRsdt = NULL;

inline uint8_t AcpiTestRsdp(void *ptr) {
  uint32_t i, sum;

  if(memcmp(ptr, "RSD PTR ", 8) != 0) {
    return 0;
  }

  for(i = 0, sum = 0; i < 20; sum += *((uint8_t *) ((uint32_t) ptr + i)), i++);
  if((sum & 0xFF) != 0x00) {
    return 0;
  } else {
    return 1;
  }
}

void AcpiFindRsdp() {
  uint16_t *ptr;
  uint32_t i, sum;

  // Search first in the first 1KB of the EBDA
  // 16 byte boundaries
  BiosEbda = (void *) ((*((uint16_t *) 0x40E)) << 4);
  for(ptr = BiosEbda; ptr < (uint16_t *) (BiosEbda + 1024); ptr += 16) {
    if(AcpiTestRsdp(ptr)) {
      AcpiRsdp = (AcpiRsdp_t *) ptr;
      break;
    }
  }

  if(AcpiRsdp == NULL) {
    // Search 0xE0000 to 0xFFFFF if not found in the EBDA
    for(ptr = (uint16_t *) 0xE0000; ptr < (uint16_t *) 0xFFFFF; ptr += 16) {
      if(AcpiTestRsdp(ptr)) {
        AcpiRsdp = (AcpiRsdp_t *) ptr;
        break;
      }
    }
  }

  if(AcpiRsdp == NULL) {
    // RSDP not found
    HALT();
  }

  AcpiRsdt = (AcpiRsdt_t *) AcpiRsdp->rsdt_addr;
  if(memcmp("RSDT", AcpiRsdt->hdr.signature, 4) != 0) {
    HALT();
  } else if(AcpiRsdt->hdr.length < 36 || (AcpiRsdt->hdr.length - 36) % 4 != 0) {
    HALT();
  }

  // RSDT checksum
  for(i = 0, sum = 0; i < AcpiRsdt->hdr.length; sum += *((uint8_t *) ((uint32_t) AcpiRsdt + i)), i++);
  if((sum & 0xFF) != 0x00) {
    HALT();
  }
}

void *AcpiFindTable(const char *signature) {
  AcpiTableHeader_t **ptr;
  uint32_t i, sum;

  if(AcpiRsdt == NULL || strlen(signature) != 4) {
    HALT();
  }

  for(ptr = (AcpiTableHeader_t **) ((uint32_t) AcpiRsdt + 36); ptr < (AcpiTableHeader_t **) ((uint32_t) AcpiRsdt + AcpiRsdt->hdr.length); ptr = (AcpiTableHeader_t **) ((uint32_t) ptr + 4)) {
    if(memcmp((*ptr)->signature, signature, 4) == 0) {
      // Checksum
      for(i = 0, sum = 0; i < (*ptr)->length; sum += *((uint8_t *) ((uint32_t) *ptr + i)), i++);
      if((sum & 0xFF) != 0x00) {
        HALT();
      }
      return (*ptr);
    }
  }

  return NULL;
}

uint8_t AcpiParseApic() {
  AcpiApic_t *apic;
  uint8_t structure_type, structure_length;
  uint32_t i;

  apic = AcpiFindTable("APIC");
  for(i = 44; i < apic->hdr.length; i += structure_length) {
    structure_type = *((uint8_t *) apic + i);
    structure_length = *((uint8_t *) apic + i + 1);

    if(structure_type >= 0xD && structure_type <= 0x7F) {
      // Reserved - skip
      continue;
    } else if(structure_type >= 0x80 && structure_type <= 0xFF) {
      // Reserved for OEM use - skip
      continue;
    } else if(structure_type == 0x00) {
      // Local APIC
      if(!ApicLocalAdd((AcpiApicLocal_t *) ((uint32_t) apic + i))) {
        return 0;
      }
    } else if(structure_type == 0x01) {
      // I/O APIC
      if(!ApicIoAdd((AcpiApicIo_t *) ((uint32_t) apic + i))) {
        return 0;
      }
    } else if(structure_type == 0x02) {
      // TODO Interrupt Source Override
    } else if(structure_type == 0x03) {
      // TODO NMI Source
    } else if(structure_type == 0x04) {
      // TODO Local APIC NMI
    } else if(structure_type == 0x05) {
      // TODO Local APIC Address Override
    } else if(structure_type == 0x06) {
      // TODO I/O SAPIC
    } else if(structure_type == 0x07) {
      // TODO Local SAPIC
    } else if(structure_type == 0x08) {
      // TODO Platform Interrupt Sources
    } else if(structure_type == 0x09) {
      // TODO Processor Local x2APIC
    } else if(structure_type == 0x0A) {
      // TODO Local x2APIC NMI
    } else if(structure_type == 0x0B) {
      // TODO GIC
    } else if(structure_type == 0x0C) {
      // TODO GICD
    } else {
      printf("Unrecognised entry in MADT (type 0x%X)\n", structure_type);
    }
  }

  if(i != apic->hdr.length) {
    return 0;
  }

  // TODO Check flags. If 8259 then disable.

  return 1;
}