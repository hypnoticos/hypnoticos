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

uint64_t KernelFunctionRead(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi) {
  char *pa;
  FunctionRead_t *data;

  // Translate va to pa
  pa = GET_PA(rbx);
  if(pa == NULL) {
    WARNING();
    return -1;
  }

  data = malloc(sizeof(FunctionRead_t));
  data->fd = (int) rax;
  data->buffer = pa;
  data->count = (size_t) rcx;
  data->offset = 0;

  KernelFunctionSuspend(p, DISPATCHER_SUSPEND_READ, data);

  return 0;
}

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
