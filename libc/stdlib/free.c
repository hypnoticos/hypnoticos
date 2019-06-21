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

#include <stdio.h>
#include <string.h>
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>

void free(void *addr) {
  MemoryTable_t *mt;

  if((mt = MemoryFind(addr)) == NULL) {
    printf("free: address does not refer to the start of a block of allocated memory\n");
    HALT();
  }

  mt->status = 0;
  memset(mt->function, 0, MEMORY_TABLE_FUNCTION_LABEL_SIZE);
  mt->line = 0;
}

#else

#include <stdio.h>
#include <hypnoticos/unimplemented.h>  // TODO Remove this when implemented

// TODO
void free(void *addr) {
  UNIMPLEMENTED();
}

#endif
