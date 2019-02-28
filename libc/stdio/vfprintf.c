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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

int vfprintf(FILE *f, const char *format, va_list va) {
  uint32_t arg_uint, count = 0, i = 0, i2, i3;
  char *arg_string, buffer[HYPNOTICOS_MAX_DIGITS + 1];

  while(format[i] != 0) {
    if(format[i] == '%' && format[i + 1] != 0) {
      i++;
      if(format[i] == 'd' || format[i] == 'i') {
        fputc(format[i], f); // TODO
        count++;
      } else if(format[i] == 'p' || format[i] == 'u' || format[i] == 'x' || format[i] == 'X') {
        arg_uint = va_arg(va, unsigned int);
        if(arg_uint == 0) {
          fputc('0', f);
          count++;
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

          fputs(buffer + i3, f);
          count += strlen(&buffer[i3]);
        }
      } else if(format[i] == 's') {
        arg_string = va_arg(va, char *);
        fputs(arg_string, f);
        count += strlen(arg_string);
      } else if(format[i] == '%') {
        fputc('%', f);
        count++;
      } else {
        fputc(format[i], f); // TODO
        count++;
      }
    } else {
      fputc(format[i], f);
      count++;
    }

    i++;
  }

  return count;
}
