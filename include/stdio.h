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

// TODO. unistd.h?
#define STDIN_FILENO      0
#define STDOUT_FILENO     1
#define STDERR_FILENO     2

#define stdin             &stdstreams[0]
#define stdout            &stdstreams[1]
#define stderr            &stdstreams[2]

#define NULL              0

#define EOF               -1

// TODO
typedef struct {
  uint32_t file_no;
} FILE;

int fclose(FILE *f);
int feof(FILE *f);
char *fgets(char *buffer, int count, FILE *f);
FILE *fopen(const char *path, const char *mode);
int fprintf(FILE *f, const char *format, ...);
int fputc(int chr, FILE *f);
int fputs(const char *s, FILE *f);
int printf(const char *format, ...);
int putchar(int chr);
int puts(const char *str);
int vfprintf(FILE *f, const char *format, va_list va); // TODO First parameter must be FILE *f

extern FILE stdstreams[3];

#endif
