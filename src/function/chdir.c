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
#include <stdlib.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/fs.h>

/**
 * @brief Change the current working directory for the process.
 * 
 * @param p The process struct for this process.
 * @param new_dir The new directory.
 * @return uint64_t Returns 0 on success, -1 on error.
 */
uint64_t KernelFunctionChangeCurrentWorkingDirectory(DispatcherProcess_t *p, uint64_t new_dir) {
  char *new_dir_pa, consolidated_path[FS_MAX_PATH_SIZE+1];

  // Get new_dir
  new_dir_pa = GET_PA(new_dir);
  if(new_dir_pa == NULL) {
    WARNING();
    return -1;
  }

  // Find the absolute path for the new working directory
  if(PathConsolidate(new_dir_pa, consolidated_path) != 1) {
    WARNING();
    return -1;
  }

  // TODO Check if the new working directory is valid

  // Set the new working directory
  p->working_directory = realloc(p->working_directory, strlen(consolidated_path) + 1);
  if(p->working_directory == NULL) {
    WARNING();
    return -1;
  }
  strcpy(p->working_directory, consolidated_path);

  return 0;
}
