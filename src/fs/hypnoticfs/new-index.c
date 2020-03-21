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

uint8_t FsNewIndex_HypnoticFS(FsRoot_t *root, const char *path, char **path_short, uint8_t type) {
  uint32_t level, max_level, name_length;
  uint64_t index_data_offset, parent_offset = 0;
  HypnoticFS_Index_t index;

  // Check if file already exists
  if(Fs_HypnoticFS_GetIndex(root, path_short, &index) != 0) {
    WARNING();
    return 0;
  }

  // Iterate through path_short to check each directory exists
  for(max_level = 0; path_short[max_level] != NULL; max_level++);
  if(max_level > 1) {
    for(level = 0; level < max_level - 1; level++) {
      // Get the index
      if(Fs_HypnoticFS_GetIndex(root, path_short, &index) == 0) {
        WARNING();
        return 0;
      }

      // Check if the index is a directory
      if(index.type != HYPNOTICFS_TYPE_DIRECTORY) {
        WARNING();
        return 0;
      }

      parent_offset = index.offset;
    }
  }

  // Check length of path
  if((name_length = strlen(path_short[max_level - 1])) > HYPNOTICFS_FILENAME_SIZE || name_length == 0) {
    WARNING();
    return 0;
  }

  // Get a new index
  if((index_data_offset = Fs_HypnoticFS_GetEmptyIndex(root)) == 0) {
    WARNING();
    return 0;
  }

  // Get offset for the data
  if((index.offset = Fs_HypnoticFS_FindSpace(root, 0)) == 0) {
    WARNING();
    return 0;
  }

  memset(index.name, 0, HYPNOTICFS_FILENAME_SIZE);
  memcpy(index.name, path_short[max_level - 1], name_length);
  index.parent_offset = parent_offset;
  index.size = 0;
  index.type = type;

  // Write index
  if(!Fs_HypnoticFS_UpdateIndex(root, index_data_offset, &index)) {
    WARNING();
    return 0;
  } else {
    return 1;
  }
}
