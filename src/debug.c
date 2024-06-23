//
// HypnoticOS
// Copyright (C) 2019, 2024  jk30
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
#include <stdlib.h>
#include <string.h>
#include <hypnoticos/cpu.h>

void printf_debug(const char *format, ...) {
  int length, i;
  char *buffer, buffer_initial;
  va_list va;

  va_start(va, format);
  length = vsnprintf(&buffer_initial, 0, format, va);
  va_end(va);

  buffer = malloc(length + 1);

  va_start(va, format);
  vsnprintf(buffer, length + 1, format, va);
  va_end(va);

  for(i = 0; i < length; i++) {
    IoPort8Out(0xE9, buffer[i]);
  }
  IoPort8Out(0xE9, '\n');

  free(buffer);
}

void puts_debug(const char *text) {
  for(int i = 0; i < strlen(text); i++) {
    IoPort8Out(0xE9, text[i]);
  }
  IoPort8Out(0xE9, '\n');
}
