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
 * Clean up a KernelFunctionDirectoryGet instance.
 * @param  p             The process.
 * @param  lock_entry_id The lock entry ID.
 * @return               Always returns 0.
 */
uint64_t KernelFunctionDirectoryDone(DispatcherProcess_t *p, uint64_t lock_entry_id)
{
  DispatcherOpenIndex_t *lock_entry;

  // Get the index lock entry
  if((lock_entry = DispatcherIndexLockRetrieve(p, lock_entry_id)) == NULL)
    return 0;

  // TODO Clean up lock_entry->directory_data

  DispatcherIndexLockDone(lock_entry);

  return 0;
}
