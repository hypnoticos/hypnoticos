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
void KeyboardInit();
void KeyboardIrq();

#endif
