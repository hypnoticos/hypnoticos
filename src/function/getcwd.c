//
// HypnoticOS
// Copyright (C) 2024  jk30
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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>

/**
 * Get the current working directory for the process.
 * @param  p      The process struct for this process.
 * @param  buffer The buffer which the current working directory will be recorded to.
 * @param  size   The size of the buffer.
 * @return        Returns the buffer on success or NULL on error.
 */
uint64_t KernelFunctionGetCurrentWorkingDirectory(DispatcherProcess_t *p, uint64_t buffer, uint64_t size) {
  char *buffer_pa;

  // Get buffer address
  buffer_pa = GET_PA(buffer);
  if(buffer_pa == NULL) {
    WARNING();
    return NULL;
  }

  uint32_t copy_size = size;
  if(strlen(p->working_directory) < size) {
    copy_size = strlen(p->working_directory);
  }

  strncpy(buffer_pa, p->working_directory, copy_size);

  return buffer;
}
