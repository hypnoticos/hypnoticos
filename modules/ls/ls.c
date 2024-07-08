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

#include <stdio.h>
#include <stddef.h>
#include <directory.h>
#include <unistd.h>

#define MAX_INDEX_TYPE      3
#define CWD_MAX             100

int main(int argc, char **argv) {
  Directory_t *directory_ptr;
  FsIndex_t entry;
  uint64_t result;
  char * const index_types[MAX_INDEX_TYPE] = { "Unknown", "file", "directory" };
  char cwd[CWD_MAX + 1];

  if(getcwd(cwd, CWD_MAX) == NULL) {
    printf("Error.\n");
    return 1;
  }

  if((directory_ptr = DirectoryGet(cwd)) == NULL) {
    printf("Error.\n");
    return 1;
  }

  printf("Contents of %s:\n", cwd);
  while((result = DirectoryEntry(directory_ptr, &entry)) == 1) {
    char *type;
    if(entry.type < MAX_INDEX_TYPE) {
      type = index_types[entry.type];
    } else {
      type = index_types[0];
    }

    printf("%s - ", entry.name);
    if(entry.type == INDEX_TYPE_DIRECTORY) {
      printf("%s\n", type);
    } else {
      printf("%s - %u bytes\n", type, (unsigned int) entry.size);
    }
  }

  if(result != DIRECTORY_END) {
    printf("Error.\n");
  }

  DirectoryDone(directory_ptr);

  return 0;
}
