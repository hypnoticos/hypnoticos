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

#include <stdlib.h>
#include <stddef.h>
#include <hypnoticos/fs.h>
#include <hypnoticos/fs/hypnoticfs.h>

/**
 * This function checks if an index exists, checks that it's a directory and
 * returns an array containing the indices within a given path.
 * @param  root       The file system root.
 * @param  path       The path.
 * @param  path_short An array containing each element of the path.
 * @return            NULL on failure, or the array on success.
 */
FsIndex_t **FsList_HypnoticFS(FsRoot_t *root, const char *path, char **path_short)
{
  HypnoticFS_Index_t directory_index;
  uint64_t directory_offset;
  uint64_t fs_index_size;
  FsIndex_t **to_ret = NULL;
  uint64_t to_ret_size = 1;
  uint8_t is_root_dir = 0;

  if(path_short[0] == NULL)
    is_root_dir = 1;

  if(!is_root_dir && (directory_offset = Fs_HypnoticFS_GetIndex(root, path_short, &directory_index)) < INDEX_START)
    return NULL;

  if(!is_root_dir && directory_index.type != HYPNOTICFS_TYPE_DIRECTORY)
    goto err_cleanup_directory_index;

  if((fs_index_size = Fs_HypnoticFS_GetIndexSize(root)) < INDEX_START)
    goto err_cleanup_directory_index;

  if((to_ret = malloc(sizeof(FsIndex_t *))) == NULL)
    goto err_cleanup_directory_index;

  to_ret[0] = NULL;

  for(uint64_t i = INDEX_START;
   i + sizeof(HypnoticFS_Index_t) < fs_index_size;
   i += sizeof(HypnoticFS_Index_t)) {
     HypnoticFS_Index_t entry;
     FsIndex_t *to_ret_index;

    if(!StorageRead(root, i, sizeof(HypnoticFS_Index_t), &entry))
      goto err_cleanup_to_ret;

    if(!is_root_dir && entry.parent_offset != directory_offset) {
      continue;
    } else if(is_root_dir && entry.parent_offset != 0) {
      continue;
    }

    if((to_ret_index = Fs_HypnoticFS_GenerateFsIndex(&entry)) == NULL)
      goto err_cleanup_to_ret;

    to_ret = realloc(to_ret, sizeof(FsIndex_t *) * (to_ret_size + 1));
    to_ret[to_ret_size - 1] = to_ret_index;
    to_ret[to_ret_size] = NULL;
    to_ret_size++;
  }

err_cleanup_to_ret:
  // TODO
err_cleanup_directory_index:
  // TODO Clean up directory_index
  return to_ret;
}
