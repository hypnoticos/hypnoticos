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

#include <hypnoticos/fs.h>
#include <hypnoticos/devices/storage.h>
#include <hypnoticos/fs/hypnoticfs.h>
#include <hypnoticos/hypnoticos.h>
#include <string.h>

uint64_t Fs_HypnoticFS_GetIndexSize(FsRoot_t *root) {
  uint64_t index_size;

  if(!StorageRead(root, MAGIC_SIZE, sizeof(uint64_t), (void *) (&index_size))) {
    WARNING();
    return 0;
  } else {
    return index_size;
  }
}
