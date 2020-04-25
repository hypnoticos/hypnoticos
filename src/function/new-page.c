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

#include <hypnoticos/memory.h>
#include <hypnoticos/dispatcher.h>

/**
 * Creates one or more new pages for the process's heap.
 * @param  p     The process struct for the process for which new pages will be
 *               created.
 * @param  count The number of new pages to be created.
 * @return       The start address for the new pages created.
 */
uint64_t KernelFunctionNewPage(DispatcherProcess_t *p, uint64_t count)
{
  uint64_t i, r;

  r = (uint64_t) p->heap_addr + p->heap_size;

  for(i = 0; i < (count / 0x1000) + 1; i++) {
    if(DispatcherProcessAllocatePage(p, p->heap_addr + p->heap_size, 0, PAGING_USER | PAGING_RW | PAGING_PRESENT) == NULL) {
      // TODO Clean up
      return (uint64_t) NULL;
    }
    p->heap_size += 0x1000;
  }

  return r;
}
