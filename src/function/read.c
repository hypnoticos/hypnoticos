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

#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>
#include <hypnoticos/devices.h>
#include <hypnoticos/keyboard-codes.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/unimplemented.h>

/**
 * Read bytes from a file descriptor into a buffer (note that the function
 * suspends the process).
 * @param  p           The process struct for this process.
 * @param  fd          The file descriptor.
 * @param  buffer_addr The buffer.
 * @param  count       The number of bytes to read.
 */
void KernelFunctionRead(DispatcherProcess_t *p, uint64_t fd, uint64_t buffer_addr, uint64_t count)
{
  char *pa;
  FunctionRead_t *data;

  // Translate va to pa
  pa = GET_PA(buffer_addr);
  if(pa == NULL) {
    WARNING();
    return;
  }

  data = malloc(sizeof(FunctionRead_t));
  data->fd = (int) fd;
  data->buffer = pa;
  data->count = (size_t) count;
  data->offset = 0;

  KernelFunctionSuspend(p, DISPATCHER_SUSPEND_READ, data);

  __builtin_unreachable();
}

/**
 * Perform the read, and test whether the read has finished so that the
 * suspension can be lifted.
 * @param p The process struct for this process.
 */
void KernelFunctionRead_SuspendTest(DispatcherProcess_t *p) {
  FunctionRead_t *data;
  char kb, output;

  // TODO Lock struct

  data = p->suspend_data;

  if(data->fd != STDIN_FILENO) {
    HALT();
  }

  if(KeyboardBufferGet_Keep() == KEY_EMPTY) {
    return;
  }
  kb = KeyboardBufferGet();

  if((kb & KEY_PRESSED) == 0x00) {
    // Key released
    return;
  }

  // Remove KEY_PRESSED bit
  kb = kb & (~KEY_PRESSED);

  switch(kb) {
    case KEY_A:
    output = 'a';
    break;

    case KEY_B:
    output = 'b';
    break;

    case KEY_C:
    output = 'c';
    break;

    case KEY_D:
    output = 'd';
    break;

    case KEY_E:
    output = 'e';
    break;

    case KEY_F:
    output = 'f';
    break;

    case KEY_G:
    output = 'g';
    break;

    case KEY_H:
    output = 'h';
    break;

    case KEY_I:
    output = 'i';
    break;

    case KEY_J:
    output = 'j';
    break;

    case KEY_K:
    output = 'k';
    break;

    case KEY_L:
    output = 'l';
    break;

    case KEY_M:
    output = 'm';
    break;

    case KEY_N:
    output = 'n';
    break;

    case KEY_O:
    output = 'o';
    break;

    case KEY_P:
    output = 'p';
    break;

    case KEY_Q:
    output = 'q';
    break;

    case KEY_R:
    output = 'r';
    break;

    case KEY_S:
    output = 's';
    break;

    case KEY_T:
    output = 't';
    break;

    case KEY_U:
    output = 'u';
    break;

    case KEY_V:
    output = 'v';
    break;

    case KEY_W:
    output = 'w';
    break;

    case KEY_X:
    output = 'x';
    break;

    case KEY_Y:
    output = 'y';
    break;

    case KEY_Z:
    output = 'z';
    break;

    case KEY_SPACE:
    output = ' ';
    break;

    case KEY_BACKSPACE:
    output = '\b';
    break;

    case KEY_RETURN:
    output = '\n';
    break;

    case KEY_MAIN_FORWARDSLASH:
    output = '/';
    break;

    case KEY_KEYPAD_FORWARDSLASH:
    output = '/';
    break;

    case KEY_MAIN_PERIOD:
    output = '.';
    break;

    case KEY_KEYPAD_PERIOD:
    output = '.';
    break;

    case KEY_MAIN_0:
    output = '0';
    break;

    case KEY_MAIN_1:
    output = '1';
    break;

    case KEY_MAIN_2:
    output = '2';
    break;

    case KEY_MAIN_3:
    output = '3';
    break;

    case KEY_MAIN_4:
    output = '4';
    break;

    case KEY_MAIN_5:
    output = '5';
    break;

    case KEY_MAIN_6:
    output = '6';
    break;

    case KEY_MAIN_7:
    output = '7';
    break;

    case KEY_MAIN_8:
    output = '8';
    break;

    case KEY_MAIN_9:
    output = '9';
    break;

    case KEY_KEYPAD_0:
    output = '0';
    break;

    case KEY_KEYPAD_1:
    output = '1';
    break;

    case KEY_KEYPAD_2:
    output = '2';
    break;

    case KEY_KEYPAD_3:
    output = '3';
    break;

    case KEY_KEYPAD_4:
    output = '4';
    break;

    case KEY_KEYPAD_5:
    output = '5';
    break;

    case KEY_KEYPAD_6:
    output = '6';
    break;

    case KEY_KEYPAD_7:
    output = '7';
    break;

    case KEY_KEYPAD_8:
    output = '8';
    break;

    case KEY_KEYPAD_9:
    output = '9';
    break;

    default:
    return;
  }

  data->buffer[data->offset] = output;
  data->offset++;

  if(data->offset >= data->count) {
    KernelFunctionDone(p, data->offset);
    free(data);
  }
}
