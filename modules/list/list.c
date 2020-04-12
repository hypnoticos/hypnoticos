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

int main(int argc, char **argv) {
  Directory_t *directory_ptr;
  FsIndex_t entry;
  uint64_t result;

  if((directory_ptr = DirectoryGet("/bin")) == NULL) {
    printf("Error.\n");
    return 1;
  }

  printf("Contents of /bin:\n");
  while((result = DirectoryEntry(directory_ptr, &entry)) == 1) {
    printf("%u bytes, type 0x%X\n", entry.size, entry.type);
  }

  if(result != DIRECTORY_END) {
    printf("Error.\n");
  }

  DirectoryDone(directory_ptr);

  return 0;
}
