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

#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/fs.h>
#include <hypnoticos/fs/hypnoticfs.h>

uint64_t FsWrite_HypnoticFS(FsRoot_t *root, const char *path, char **path_short, uint64_t offset, uint64_t size, uint8_t *src) {
  HypnoticFS_Index_t index;
  uint64_t index_data_offset;

  // Find the index
  if((index_data_offset = Fs_HypnoticFS_GetIndex(root, path_short, &index)) == 0) {
    WARNING();
    return 0;
  }

  // Is the current size exceeded?
  if(offset + size > index.size) {
    if(Fs_HypnoticFS_Resize(root, index_data_offset, &index, offset + size) == 0) {
      WARNING();
      return 0;
    }
  }

  // Write data
  if(!StorageWrite(root, index.offset + offset, size, src)) {
    WARNING();
    return 0;
  } else {
    return 1;
  }
}
