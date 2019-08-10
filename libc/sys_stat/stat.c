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

#ifdef _HYPNOTICOS_KERNEL

#include <hypnoticos/fs.h>
#include <sys/stat.h>
#include <stddef.h>

int stat(const char *file, struct stat *dest) {
  FsIndex_t *index;

  if((index = FsDetailsGet(file)) == NULL) {
    return -1;
  } else {
    dest->st_size = index->size;
    dest->st_mode = S_IRWXU | S_IRWXG | S_IRWXO;
    if(index->type == INDEX_TYPE_FILE) {
      dest->st_mode = dest->st_mode | S_IFDIR;
    } else if(index->type == INDEX_TYPE_DIRECTORY) {
      dest->st_mode = dest->st_mode | S_IFREG;
    } else {
      return -1;
    }
    return 0;
  }
}

#else

#include <hypnoticos/interface.h>
#include <hypnoticos/function-codes.h>
#include <sys/stat.h>

int stat(const char *file, struct stat *dest) {
  return KernelFunctionInterface((uint64_t) file, (uint64_t) dest, 0, 0, 0, KERNEL_FUNCTION_STAT);
}

#endif
