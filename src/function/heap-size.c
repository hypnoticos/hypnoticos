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
 * Returns the size of the process's heap.
 * @param  p The process struct for the process.
 * @return   The size of the process's heap.
 */
uint64_t KernelFunctionHeapSize(DispatcherProcess_t *p)
{
  return p->heap_size;
}
