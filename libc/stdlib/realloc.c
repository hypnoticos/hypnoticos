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

#ifdef _HYPNOTICOS_KERNEL

#include <stdlib.h>
#include <string.h>
#include <hypnoticos/memory.h>

void *realloc(void *addr, size_t new_size) {
  void *r;
  MemoryTable_t *mt;

  if((mt = MemoryFind(addr)) == NULL) {
    return NULL;
  }

  r = __malloc_align(new_size, ALIGN_NONE, mt->function, mt->line);

  if(mt->size > new_size) {
    memcpy(r, addr, new_size);
  } else {
    memcpy(r, addr, mt->size);
    memset(r + mt->size, 0, new_size - mt->size);
  }

  free(addr);

  return r;
}

#else

#include <stdlib.h>
#include <hypnoticos/unimplemented.h> // TODO Remove once implemented

// TODO
void *realloc(void *addr, size_t new_size) {
  (void) addr;
  (void) new_size;

  UNIMPLEMENTED();

  return NULL;
}

#endif
