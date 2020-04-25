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

#include <stddef.h>
#include <unistd.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>
#include <hypnoticos/hypnoticos.h>

/**
 * Write a specified number of bytes to a file descriptor from a buffer.
 * @param  p           The process struct for this process.
 * @param  fd          The file descriptor.
 * @param  buffer_addr The buffer.
 * @param  count       The number of bytes.
 * @return             The result of write()
 */
uint64_t KernelFunctionWrite(DispatcherProcess_t *p, uint64_t fd, uint64_t buffer_addr, uint64_t count)
{
  char *pa;

  // Translate va to pa
  pa = GET_PA(buffer_addr);
  if(pa == NULL) {
    WARNING();
    return 0;
  }

  // Send to output buffer
  return write(fd, pa, count);
}
