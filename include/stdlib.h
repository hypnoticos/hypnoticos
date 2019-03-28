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

#ifndef __STDLIB_H
#define __STDLIB_H

#include <stddef.h>

void *calloc(size_t count, size_t size);
void free(void *addr);

#ifdef _HYPNOTICOS_KERNEL
#include <hypnoticos/memory.h>
#define malloc(a)       __malloc(a, __FUNCTION__, __LINE__)
#else
void *malloc(size_t size);
#endif

void *realloc(void *addr, size_t new_size);

#endif
