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

#ifndef __HYPNOTICOS_CPU_H
#define __HYPNOTICOS_CPU_H

#include <stdint.h>

typedef struct _Tss_t Tss_t;
typedef struct _IdtGate_t IdtGate_t;

struct _Tss_t {
   uint32_t prev_tss;
   uint32_t esp0;
   uint32_t ss0;
   uint32_t esp1;
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;
   uint32_t cs;
   uint32_t ss;
   uint32_t ds;
   uint32_t fs;
   uint32_t gs;
   uint32_t ldt;
   uint16_t trap;
   uint16_t iomap_base;
} __attribute__((packed));

struct _IdtGate_t {
  uint16_t offset_low;
  uint16_t ss;
  uint16_t flags;
  uint16_t offset_high;
} __attribute__((packed));

typedef struct _AcpiRsdp_t AcpiRsdp_t;
struct _AcpiRsdp_t {
  uint8_t signature[8];
  uint8_t checksum;
  uint8_t oem_id[6];
  uint8_t revision;
  uint32_t rsdt_addr;
  uint32_t length;
  uint32_t xsdt_addr_high;
  uint32_t xsdt_addr_low;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((packed));

typedef struct _AcpiTableHeader_t AcpiTableHeader_t;
typedef struct _AcpiRsdt_t AcpiRsdt_t;
typedef struct _AcpiApicIo_t AcpiApicIo_t;
typedef struct _AcpiApicLocal_t AcpiApicLocal_t;

struct _AcpiTableHeader_t {
  uint8_t signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  uint8_t oem_id[6];
  uint8_t oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed));

struct _AcpiRsdt_t {
  AcpiTableHeader_t hdr;
  // Entries are here
} __attribute__((packed));

struct _AcpiApicIo_t {
  uint8_t type;
  uint8_t length;
  uint8_t apic_id;
  uint8_t reserved;
  uint32_t addr;
  uint32_t global_system_interrupt_base;
} __attribute__((packed));

struct _AcpiApicLocal_t {
  uint8_t type;
  uint8_t length;
  uint16_t acpi_processor_id;
  uint8_t apic_id;
  uint32_t flags;
} __attribute__((packed));

#define APIC_LOCAL_OFFSET_SIVR          0x0F0
#define APIC_LOCAL_OFFSET_TIMER_DCR     0x3E0
#define APIC_LOCAL_OFFSET_TIMER_LVT     0x320
#define APIC_LOCAL_OFFSET_TIMER_ICR     0x380

#define APIC_LOCAL_DCR_2                0x00

#define APIC_LOCAL_VECTOR_TIMER         0xA0
#define APIC_LOCAL_VECTOR_SPURIOUS      0xF0

void AcpiFindRsdp();
void *AcpiFindTable(const char *signature);
uint8_t AcpiParseApic();
uint8_t ApicIoAdd(AcpiApicIo_t *ptr);
uint8_t ApicIoInit();
uint8_t ApicLocalCheck();
void ApicLocalSetUpTimer();
void CpuChecks();
uint32_t *Cpuid(uint32_t eax_input);
uint32_t *MsrRead(uint32_t ecx_input);
void MsrWrite(uint32_t ecx, uint32_t edx, uint32_t eax);
void IdtInit();
void TssInit();

#endif
