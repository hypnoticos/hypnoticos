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
#include <stdlib.h>
#include <fcntl.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>
#include <hypnoticos/devices.h>
#include <hypnoticos/keyboard-codes.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/unimplemented.h>

uint64_t KernelFunctionOpen(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi) {
  char *filename;
  int flags;
  mode_t mode;

  // Translate va to pa
  filename = GET_PA(rax);
  if(filename == NULL) {
    WARNING();
    return -1;
  }

  flags = rbx;
  mode = rcx;

  // TODO Open file

  if((flags & O_WRONLY)) {
    // TODO Unsupported
    WARNING();
    return -1;
  }

  return DispatcherProcessNewFd(p, filename, flags, mode);
}
