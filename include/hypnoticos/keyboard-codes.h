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

#ifndef HYPNOTICOS_KEYBOARD_CODES_H
#define HYPNOTICOS_KEYBOARD_CODES_H

#define ADD_KEY(k)      KeyboardBufferQueue(k)
#define CHECK_KEY_IF_1(compare0, to_add)    CHECK_KEY_IF_6((compare0), 0, 0, 0, 0, 0, (to_add))
#define CHECK_KEY_IF_2(compare0, compare1, to_add)    CHECK_KEY_IF_6((compare0), (compare1), 0, 0, 0, 0, (to_add))
#define CHECK_KEY_IF_4(compare0, compare1, compare2, compare3, to_add)    CHECK_KEY_IF_6((compare0), (compare1), (compare2), (compare3), 0, 0, (to_add))
#define CHECK_KEY_IF_6(compare0, compare1, compare2, compare3, compare4, compare5, to_add)       if(key[0] == (compare0) && key[1] == (compare1) && key[2] == (compare2) && key[3] == (compare3) && key[4] == (compare4) && key[5] == (compare5)) ADD_KEY((to_add))
#define CHECK_KEY_ELSE_IF_1 else CHECK_KEY_IF_1
#define CHECK_KEY_ELSE_IF_2 else CHECK_KEY_IF_2
#define CHECK_KEY_ELSE_IF_4 else CHECK_KEY_IF_4
#define CHECK_KEY_ELSE_IF_6 else CHECK_KEY_IF_6

#define KEY_PRESSED         0x80 // Highest bit is reserved for key press state
#define KEY_RELEASED        0x00
#define KEY_EMPTY           0x00

#define KEY_ESC             0x01
#define KEY_BACKSPACE       0x02
#define KEY_TAB             0x03
#define KEY_RETURN          0x04
#define KEY_KEYPAD_RETURN   0x05
#define KEY_CTRL_LEFT       0x06
#define KEY_CTRL_RIGHT      0x07
#define KEY_SHIFT_LEFT      0x08
#define KEY_SHIFT_RIGHT     0x09
#define KEY_ALT_LEFT        0x0A
#define KEY_ALT_RIGHT       0x0B
#define KEY_SPACE           0x0C
#define KEY_LOCK_CAPS       0x0D
#define KEY_LOCK_NUM        0x0E
#define KEY_LOCK_SCROLL     0x0F
#define KEY_PAUSE           0x10
#define KEY_F1              0x11
#define KEY_F2              0x12
#define KEY_F3              0x13
#define KEY_F4              0x14
#define KEY_F5              0x15
#define KEY_F6              0x16
#define KEY_F7              0x17
#define KEY_F8              0x18
#define KEY_F9              0x19
#define KEY_F10             0x1A
#define KEY_F11             0x1B
#define KEY_F12             0x1C
#define KEY_INSERT          0x1D
#define KEY_DELETE          0x1E
#define KEY_HOME            0x1F
#define KEY_END             0x20
#define KEY_PAGE_UP         0x21
#define KEY_PAGE_DOWN       0x22
#define KEY_ARROW_UP        0x23
#define KEY_ARROW_DOWN      0x24
#define KEY_ARROW_LEFT      0x25
#define KEY_ARROW_RIGHT     0x26
#define KEY_PRINT_SCREEN    0x27
#define KEY_MAIN_1          0x30
#define KEY_MAIN_2          0x31
#define KEY_MAIN_3          0x32
#define KEY_MAIN_4          0x33
#define KEY_MAIN_5          0x34
#define KEY_MAIN_6          0x35
#define KEY_MAIN_7          0x36
#define KEY_MAIN_8          0x37
#define KEY_MAIN_9          0x38
#define KEY_MAIN_0          0x39
#define KEY_RIGHT_QUOTE     0x3A    // '
#define KEY_MAIN_HYPHEN     0x3B    // -
#define KEY_EQUALS          0x3C    // =
#define KEY_BACKSLASH       0x3D    // '\'
#define KEY_MAIN_FORWARDSLASH    0x3E    // /
#define KEY_MAIN_PERIOD     0x3F    // .
#define KEY_COMMA           0x40    // ,
#define KEY_LEFT_SQUARE     0x41    // [
#define KEY_RIGHT_SQUARE    0x42    // ]
#define KEY_KEYPAD_0        0x43
#define KEY_KEYPAD_1        0x44
#define KEY_KEYPAD_2        0x45
#define KEY_KEYPAD_3        0x46
#define KEY_KEYPAD_4        0x47
#define KEY_KEYPAD_5        0x48
#define KEY_KEYPAD_6        0x49
#define KEY_KEYPAD_7        0x4A
#define KEY_KEYPAD_8        0x4B
#define KEY_KEYPAD_9        0x4C
#define KEY_KEYPAD_HYPHEN   0x4D    // -
#define KEY_KEYPAD_PLUS     0x4E    // +
#define KEY_KEYPAD_ASTERICKS  0x4F  // *
#define KEY_KEYPAD_FORWARDSLASH 0x50  // /
#define KEY_Q               0x51
#define KEY_W               0x52
#define KEY_E               0x53
#define KEY_R               0x54
#define KEY_T               0x55
#define KEY_Y               0x56
#define KEY_U               0x57
#define KEY_I               0x58
#define KEY_O               0x59
#define KEY_P               0x5A
#define KEY_A               0x5B
#define KEY_S               0x5C
#define KEY_D               0x5D
#define KEY_F               0x5E
#define KEY_G               0x5F
#define KEY_H               0x60
#define KEY_J               0x61
#define KEY_K               0x62
#define KEY_L               0x63
#define KEY_Z               0x64
#define KEY_X               0x65
#define KEY_C               0x66
#define KEY_V               0x67
#define KEY_B               0x68
#define KEY_N               0x69
#define KEY_M               0x6A
#define KEY_SEMICOLON       0x6B    // ;
#define KEY_HASH            0x6C    // #
#define KEY_KEYPAD_PERIOD   0x6D    // .

#endif
