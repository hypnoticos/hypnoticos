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

#include <stddef.h>
#include <stdlib.h>
#include <hypnoticos/fs.h>
#include <hypnoticos/fs/hypnoticfs.h>
#include <hypnoticos/hypnoticos.h>

uint64_t FsRead_HypnoticFS(FsRoot_t *root, const char *path, char **path_short, uint64_t offset, uint64_t size, uint8_t *dest) {
  HypnoticFS_Index_t entry;
  uint64_t read_size;

  // Get entry
  if(Fs_HypnoticFS_GetIndex(root, path_short, &entry) == 0) {
    WARNING();
    return 0;
  }

  // Limit the read to the size of the entry
  if(offset + size > entry.size) {
    read_size = entry.size - offset;
  } else {
    read_size = offset + size;
  }

  // Read
  if(StorageRead(root, entry.offset + offset, read_size, dest) != 1) {
    WARNING();
    memset(dest, 0, size);
    return 0;
  }

  // Return size read
  return read_size;
}
