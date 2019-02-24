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

#ifndef __STDIO_H
#define __STDIO_H

#include <stdarg.h>
#include <stdint.h>

// TODO
#define stdin       1
#define stdout      2
#define stderr      3

#define EOF         -1

int fputc(int chr, uint32_t f);
int fputs(const char *s, uint32_t f);
int printf(const char *format, ...);
int putchar(int chr);
int puts(const char *str);
int vfprintf(uint32_t f, const char *format, va_list va); // TODO First parameter must be FILE *f

#endif
