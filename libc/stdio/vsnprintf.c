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
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define VSNPRINTF_WRITE(c)          if(ret < count) { \
                                      dest[ret] = (c); \
                                    } \
                                    ret++;

int vsnprintf(char *dest, size_t count, const char *format, va_list va) {
  uint64_t ret = 0, i = 0, i2, i3;
  uint64_t arg_uint;
  char *arg_string, buffer[HYPNOTICOS_MAX_DIGITS + 1];
  int arg_int;

  while(format[i] != 0) {
    if(format[i] == '%' && format[i + 1] != 0) {
      i++;
      if(format[i] == 'c') {
        arg_int = va_arg(va, int);
        VSNPRINTF_WRITE(arg_int);
      } else if(format[i] == 'p' || format[i] == 'u' || format[i] == 'x' || format[i] == 'X') {
        arg_uint = va_arg(va, uint64_t); // TODO %llu
        if(arg_uint == 0) {
          VSNPRINTF_WRITE('0');
        } else {
          memset(buffer, 0, HYPNOTICOS_MAX_DIGITS + 1);
          i3 = HYPNOTICOS_MAX_DIGITS - 1;

          while(arg_uint > 0) {
            i2 = arg_uint % (format[i] == 'u' ? 10 : 16);
            if(format[i] == 'u') {
              buffer[i3] = '0' + i2;
            } else {
              buffer[i3] = (i2 < 10 ? '0' : (format[i] == 'X' ? 'A' : 'a')) + i2 - (i2 < 10 ? 0 : 10);
            }
            arg_uint = (arg_uint - i2) / (format[i] == 'u' ? 10 : 16);

            if(i3 == 0) {
              break;
            } else {
              i3--;
            }
          }

          // Print from the first non-null character
          for(i3 = 0; i3 < HYPNOTICOS_MAX_DIGITS - 1; i3++) {
            if(buffer[i3] != 0) {
              break;
            }
          }
          for(; buffer[i3] != 0; i3++) {
            VSNPRINTF_WRITE(buffer[i3]);
          }
        }
      } else if(format[i] == 's') {
        arg_string = va_arg(va, char *);
        for(i2 = 0; arg_string[i2] != 0; i2++) {
          VSNPRINTF_WRITE(arg_string[i2]);
        }
      } else if(format[i] == '%') {
        VSNPRINTF_WRITE('%');
      } else {
        VSNPRINTF_WRITE(format[i]);
      }
    } else {
      VSNPRINTF_WRITE(format[i]);
    }

    i++;
  }

  // Set final byte to NUL
  if(count > 0) {
    if(ret > count) {
      dest[count] = 0;
    } else {
      dest[ret] = 0;
    }
  }

  return ret;
}
