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
#include <stdlib.h>
#include <string.h>
#include <hypnoticos/fs.h>

void FsNewRoot(uint32_t fs_type, StorageDevice_t *storage, uint64_t storage_offset) {
  uint32_t root_count;
  FsRoot_t *new_root;

  for(root_count = 0; FsRoots[root_count].fs_type != ROOT_TYPE_END; root_count++);

  FsRoots = realloc(FsRoots, sizeof(FsRoot_t) * (root_count + 2));
  FsRoots[root_count + 1].fs_type = NULL;

  new_root = &(FsRoots[root_count]);

  new_root->fs_type = fs_type;
  new_root->fs_type_data = NULL;
  new_root->root = malloc(2);
  strcpy(new_root->root, "/");
  new_root->storage = storage;
  new_root->storage_offset = 0;
}
