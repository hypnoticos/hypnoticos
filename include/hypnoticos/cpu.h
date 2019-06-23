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

#ifndef HYPNOTICOS_CPU_H
#define HYPNOTICOS_CPU_H

#include <stdint.h>

typedef struct _Tss_t Tss_t;
typedef struct _IdtGate_t IdtGate_t;

struct _Tss_t {
  uint32_t reserved1;

  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;

  uint64_t reserved2;

  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;

  uint64_t reserved3;
  uint16_t reserved4;

  uint16_t io_map_base;
  uint8_t int_redirection[32];
  uint8_t io_permission[8192];
  uint8_t io_permission_final;
} __attribute__((packed)) __attribute__((aligned(4096)));

struct _IdtGate_t {
  uint16_t offset_0_15;
  uint16_t cs;
  uint8_t ist; // NOTE: bits 3 to 7 (inclusive) must be set to 0
  uint8_t flags;
  uint16_t offset_16_31;
  uint32_t offset_32_63;
  uint32_t reserved;
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

typedef struct _AcpiApic_t AcpiApic_t;
struct _AcpiApic_t {
  AcpiTableHeader_t hdr;
  uint32_t local_int_ctrlr_addr;
  uint32_t flags;
  // Entries are here
} __attribute__((packed));

typedef struct _AcpiHpet_t AcpiHpet_t;
struct _AcpiHpet_t {
  AcpiTableHeader_t hdr;
  uint32_t block_id;
  uint32_t addr_flags;
  uint64_t addr;
  uint8_t table;
  uint16_t minimum_ticks;
  uint8_t flags;
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
  uint8_t acpi_processor_id;
  uint8_t apic_id;
  uint32_t flags;
} __attribute__((packed));

typedef struct _TssEntries_t TssEntries_t;
struct _TssEntries_t {
  Tss_t *tss;
  void *stack;
};

#define TSS_RSP0_SIZE                   8192

#define APIC_LOCAL_OFFSET_ID            0x020
#define APIC_LOCAL_OFFSET_EOI           0x0B0
#define APIC_LOCAL_OFFSET_SIVR          0x0F0
#define APIC_LOCAL_OFFSET_ICR_L         0x300
#define APIC_LOCAL_OFFSET_ICR_H         0x310
#define APIC_LOCAL_OFFSET_TIMER_DCR     0x3E0
#define APIC_LOCAL_OFFSET_TIMER_LVT     0x320
#define APIC_LOCAL_OFFSET_TIMER_ICR     0x380

#define APIC_LOCAL_DCR_2                0x00

#define APIC_LOCAL_VECTOR_TIMER         0xA0
#define APIC_LOCAL_VECTOR_AP_START      0x02
#define APIC_LOCAL_VECTOR_SPURIOUS      0xF0
#define APIC_LOCAL_VECTOR_AP_START_INT  242

#define APIC_LOCAL_TIMER_PERIODIC       0x20000

#define IDT_IRQ_1                       49
#define IDT_IRQ_2                       50

#define CPU_BSP                         1
#define CPU_AP                          0

extern uint8_t ApInitDone;
extern volatile void *ApicLocalBase;
extern uint8_t IdtCallCurrentPrivilegeLevel;
extern Tss_t Tss;
extern uint64_t IdtCallSavedCr3;
extern uint64_t IdtCallSavedRbp;
extern uint64_t IdtCallSavedRsp;
extern uint64_t IdtCallSavedRip;
extern uint64_t IdtCallSavedRax;
extern uint64_t IdtCallSavedRbx;
extern uint64_t IdtCallSavedRcx;
extern uint64_t IdtCallSavedRdx;
extern uint64_t IdtCallSavedRsi;
extern uint64_t IdtCallSavedRdi;
extern uint64_t IdtCallSavedRflags;
extern uint64_t IdtCallSavedR8;
extern uint64_t IdtCallSavedR9;
extern uint64_t IdtCallSavedR10;
extern uint64_t IdtCallSavedR11;
extern uint64_t IdtCallSavedR12;
extern uint64_t IdtCallSavedR13;
extern uint64_t IdtCallSavedR14;
extern uint64_t IdtCallSavedR15;
extern TssEntries_t **TssEntriesAPs;

#define APIC_LOCAL_READ(offset)                (*((volatile uint32_t *) ((volatile uint64_t) (ApicLocalBase) + (offset))))
#define APIC_LOCAL_WRITE(offset, value)        (*((volatile uint32_t *) ((volatile uint64_t) (ApicLocalBase) + (offset))) = (value))

#define APIC_LOCAL_GET_ID()                           ((uint8_t) ((APIC_LOCAL_READ(APIC_LOCAL_OFFSET_ID)) >> 24))

void AcpiFindRsdp();
uint8_t AcpiParse();
uint8_t ApicIoAdd(AcpiApicIo_t *ptr);
uint8_t ApicIoInit();
void ApicIoMapIrqs();
void *ApicLocalGetAddr();
uint8_t ApicLocalParseAcpi(AcpiApicLocal_t *ptr);
uint8_t ApicLocalInit(uint8_t bsp);
void ApicLocalEoi();
void ApicLocalSetUpTimer();
void ApicLocalStartInterruptsOnAPs();
void CpuApic(uint8_t bsp);
void CpuChecks(uint8_t bsp);
uint32_t *Cpuid(uint64_t rax_input);
uint8_t HpetInit(void *addr);
void IdtInit();
extern void IdtInit();
extern void IdtSet();
uint8_t IoPort8In(uint16_t port);
void IoPort8Out(uint16_t port, uint8_t data);
uint16_t IoPort16In(uint16_t port);
void IoPort16Out(uint16_t port, uint16_t data);
uint32_t IoPort32In(uint16_t port);
void IoPort32Out(uint16_t port, uint32_t data);
uint32_t *MsrRead(uint32_t ecx_input);
void MsrWrite(uint32_t ecx, uint32_t edx, uint32_t eax);
uint8_t PciInit();
extern uint64_t RflagsGet();

#endif
