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

#include <unistd.h>

#ifdef _HYPNOTICOS_KERNEL

#include <stdint.h>
#include <hypnoticos/video-memory.h>

ssize_t write(int fd, const void *buffer, size_t count) {
  size_t i;

  // TODO
  if(fd != STDOUT_FILENO && fd != STDERR_FILENO) {
    return -1;
  }

  for(i = 0; i < count; i++) {
    VideoMemoryPutc(((char *) buffer)[i]);
  }

  return count;
}

#else

#include <hypnoticos/interface.h>
#include <hypnoticos/function-codes.h>

ssize_t write(int fd, const void *buffer, size_t count) {
  return KernelFunctionInterface(fd, (uint64_t) buffer, count, 0, 0, KERNEL_FUNCTION_WRITE);
}

#endif
