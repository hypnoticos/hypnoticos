//
// HypnoticOS
// Copyright (C) 2019, 2020, 2024  jk30
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

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

void *calloc(size_t count, size_t size);
void free(void *addr);

#if defined(_HYPNOTICOS_KERNEL) && !defined(_HYPNOTICOS_TESTS)

#include <hypnoticos/memory.h>
#define malloc(size)                  __malloc_align(size, ALIGN_NONE, __FUNCTION__, __LINE__)
#define malloc_align(size, align)     __malloc_align(size, align, __FUNCTION__, __LINE__)

#else

void exit(int code);
void itoa(int64_t i, char *a);
void itoh(int64_t i, char *a);
void *malloc(size_t size);

#ifdef _HYPNOTICOS_TESTS
#define malloc_align(size, align)     malloc(size)
#endif

#endif

void *realloc(void *addr, size_t new_size);

#endif
