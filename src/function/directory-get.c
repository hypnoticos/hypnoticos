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

uint64_t KernelFunctionDirectoryGet(DispatcherProcess_t *p, uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t rsi, uint64_t rdi) {
  char *path;
  DispatcherOpenIndex_t *lock_entry;

  // Get path
  path = GET_PA(rax);
  if(path == NULL) {
    WARNING();
    return 0;

  // Attempt to lock
  if((lock_entry = DispatcherIndexLockAttempt(p, path)) == NULL) {
    WARNING();
    return 0;
  }

  // Load directory data into memory
  if((lock_entry->directory_data = FsList(path)) == NULL) {
    WARNING();
    return 0;
  }

  return lock_entry->open_index_id;
}
