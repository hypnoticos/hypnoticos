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

#ifndef HYPNOTICOS_DEVICES_H
#define HYPNOTICOS_DEVICES_H

#include <stdint.h>

#define KEYBOARD_STATE_LOCK_NUM         0x0001
#define KEYBOARD_STATE_LOCK_CAPS        0x0002
#define KEYBOARD_STATE_LOCK_SCROLL      0x0004
#define KEYBOARD_STATE_SHIFT_LEFT       0x0008
#define KEYBOARD_STATE_SHIFT_RIGHT      0x0010
#define KEYBOARD_STATE_CTRL_LEFT        0x0020
#define KEYBOARD_STATE_CTRL_RIGHT       0x0040
#define KEYBOARD_STATE_ALT_LEFT         0x0080
#define KEYBOARD_STATE_ALT_RIGHT        0x0100

#define KEYBOARD_STATE_SET(a)           (KeyboardState = KeyboardState | (a))
#define KEYBOARD_STATE_UNSET(a)         (KeyboardState = KeyboardState ^ (a))
#define KEYBOARD_STATE_TEST(a)          (KeyboardState & (a))
#define KEYBOARD_STATE_SWAP(a)          (KEYBOARD_STATE_TEST(a) ? KEYBOARD_STATE_UNSET(a) : KEYBOARD_STATE_SET(a))
#define KEYBOARD_STATE_TEST_SHIFT()     ((KEYBOARD_STATE_TEST(KEYBOARD_STATE_SHIFT_LEFT) || KEYBOARD_STATE_TEST(KEYBOARD_STATE_SHIFT_RIGHT)))
#define KEYBOARD_STATE_TEST_CTRL()      ((KEYBOARD_STATE_TEST(KEYBOARD_STATE_CTRL_LEFT) || KEYBOARD_STATE_TEST(KEYBOARD_STATE_CTRL_RIGHT)))
#define KEYBOARD_STATE_TEST_ALT()       ((KEYBOARD_STATE_TEST(KEYBOARD_STATE_ALT_LEFT) || KEYBOARD_STATE_TEST(KEYBOARD_STATE_ALT_RIGHT)))

extern uint8_t KeyboardPresent;
extern uint8_t KeyboardState;

uint8_t KeyboardBufferGet();
uint8_t KeyboardBufferGet_Keep();
void KeyboardInit();
void KeyboardIrq();

typedef struct _PciHdr_t PciHdr_t;
typedef struct _PciHdr0_t PciHdr0_t;
typedef struct _PciCache_t PciCache_t;

struct _PciHdr_t {
  uint16_t vendor_id;
  uint16_t device_id;
  uint16_t command;
  uint16_t status;
  uint8_t revision_id;
  uint8_t prog_if;
  uint8_t subclass;
  uint8_t class;
  uint8_t cache_line_size;
  uint8_t latency_timer;
  uint8_t header_type;
  uint8_t bist;
} __attribute__((packed));

#define PCI_MAX_BARS          6
struct _PciHdr0_t {
  PciHdr_t hdr;
  uint32_t bar[PCI_MAX_BARS];
  uint32_t cardbus_cis_pointer;
  uint16_t subsystem_vendor_id;
  uint16_t subsystem_id;
  uint32_t expansion_base_rom_address;
  uint8_t capabilities_ptr;
  uint8_t reserved_1[3];
  uint32_t reserved_2;
  uint8_t int_line;
  uint8_t int_pin;
  uint8_t min_grant;
  uint8_t max_latency;
} __attribute__((packed));

struct _PciCache_t {
  uint32_t bus;
  uint32_t device;
  uint32_t function;
  PciHdr0_t hdr0;
  uint8_t handler; /*!< 0 = Not handled. 1 = Handled by kernel. 2 = Handled by driver. */
};

extern PciCache_t **PciCache;

#endif
