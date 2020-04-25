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

#include <stddef.h>
#include <hypnoticos/function.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/fs.h>
#include <hypnoticos/hypnoticos.h>

/**
 * Create an instance for use with KernelFunctionDirectoryEntry.
 * @param  p    The process struct for this process.
 * @param  path The path to the directory.
 * @return      0 on failure, or a lock entry ID on success.
 */
uint64_t KernelFunctionDirectoryGet(DispatcherProcess_t *p, uint64_t path)
{
  char *path_pa;
  DispatcherOpenIndex_t *lock_entry;

  // Get path
  path_pa = GET_PA(path);
  if(path_pa == NULL) {
    WARNING();
    return 0;
  }

  // Attempt to lock
  if((lock_entry = DispatcherIndexLockAttempt(p, path_pa)) == NULL) {
    WARNING();
    return 0;
  }

  // Load directory data into memory
  if((lock_entry->directory_data = FsList(path_pa)) == NULL) {
    WARNING();
    return 0;
  }

  return lock_entry->open_index_id;
}
