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


#ifndef _HYPNOTICOS_KERNEL
#include <hypnoticos/interface.h>
#include <hypnoticos/function-codes.h>

/**
 * Run a binary.
 * @param  path The path to the binary.
 * @param  argv The parameters, excluding the first parameter (path to binary).
 * @param  argc The number of parameters + 1.
 * @return      The process ID, or 0 on error.
 */
uint16_t Run(char *path, char **argv, int argc)
{
  return KernelFunctionInterface((uint64_t) path, (uint64_t) argv, (uint64_t) argc, 0, 0, KERNEL_FUNCTION_RUN);
}

#endif
