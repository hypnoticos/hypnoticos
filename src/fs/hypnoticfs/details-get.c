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

FsIndex_t *FsDetailsGet_HypnoticFS(FsRoot_t *root, const char *path, char **path_short) {
  HypnoticFS_Index_t *entry;
  FsIndex_t *r; // TODO Caller must free this

  entry = malloc(sizeof(HypnoticFS_Index_t));
  if(Fs_HypnoticFS_GetIndex(root, path_short, entry) == 0) {
    WARNING();
    free(entry);
    return NULL;
  }

  if((r = Fs_HypnoticFS_GenerateFsIndex(entry)) == NULL) {
    WARNING();
    free(entry);
    return NULL;
  }

  free(entry);

  return r;
}
