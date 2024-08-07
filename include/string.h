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

#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <sys/types.h>

int memcmp(const void *addr1, const void *addr2, size_t count);
void *memcpy(void *destination, const void *source, size_t count);
void *memset(void *addr, int chr, size_t count);
char *strcat(char *dest, const char *src);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *d, const char *s);
size_t strlen(const char *s);
char *strncpy(char *d, const char *s, size_t len);

#endif
