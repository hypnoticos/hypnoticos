//
// HypnoticOS
// Copyright (C) 2020  jk30
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
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>

/**
 * Run a binary.
 * @param  p    The process struct for the process which will call the binary.
 * @param  path The path of the binary.
 * @return      Returns 0 on failure, or the PID of the new process on success.
 */
uint64_t KernelFunctionRun(DispatcherProcess_t *p, uint64_t path)
{
  char *path_pa;
  DispatcherProcess_t *process;

  // Translate va to pa
  path_pa = GET_PA(path);
  if(path_pa == NULL) {
    WARNING();
    return 0;
  }

  if((process = DispatcherProcessNewFromFormat(path_pa)) == NULL) {
    return 0;
  } else {
    return process->pid;
  }
}
