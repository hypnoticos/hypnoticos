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

#include <stdlib.h>
#include <hypnoticos/memory.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>

/**
 * Run a binary.
 * @param  p    The process struct for the process which will call the binary.
 * @param  path The path of the binary.
 * @param  argv An array containing each parameter to be passed to the binary,
 *              not including what the binary will see as argv[0].
 * @param  argc The number of parameters + 1 (this argument cannot be 0).
 * @return      Returns 0 on failure, or the PID of the new process on success.
 */
uint64_t KernelFunctionRun(DispatcherProcess_t *p, uint64_t path, uint64_t argv, uint64_t argc)
{
  char *path_pa;
  char **argv_va;
  char **argv_pa = NULL;
  DispatcherProcess_t *process;

  path_pa = GET_PA(path);
  if(path_pa == NULL) {
    WARNING();
    return 0;
  }

  if(argc == 0 || argc > MAX_ARGC) {
    return 0;
  } else if(argc > 1) {
    if((argv_pa = malloc(sizeof(char *) * argc)) == NULL)
      return 0;

    if((argv_va = GET_PA(argv)) == NULL) {
      free(argv_pa);
      return 0;
    }

    for(int i = 0; i < (argc - 1); i++) {
      if((argv_pa[i] = GET_PA((uint64_t) (argv_va[i]))) == NULL) {
        WARNING();
        return 0;
      }
    }
  }

  if((process = DispatcherProcessNewFromFormat(path_pa, argv_pa, argc, p)) == NULL) {
    return 0;
  } else {
    return process->pid;
  }
}
