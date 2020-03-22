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
#include <hypnoticos/fs.h>
#include <hypnoticos/fs/hypnoticfs.h>
#include <hypnoticos/hypnoticos.h>

uint8_t FsNewIndex(const char *path, uint8_t type) {
  char **path_short;
  FsRoot_t *root;
  uint64_t r;

  // TODO Find FS root
  if((path_short = PathBreakdown(path)) == NULL) {
    return 0;
  }
  root = &(FsRoots[0]);

  // Pass to FS function
  switch(root->fs_type) {
    case ROOT_TYPE_HYPNOTICFS:
    r = FsNewIndex_HypnoticFS(root, path, path_short, type);
    break;

    default:
    WARNING();
    r = 0;
    break;
  }

  // TODO Clean up path_short

  return r;
}
