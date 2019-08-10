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
#include <string.h>
#include <hypnoticos/fs.h>
#include <hypnoticos/devices/storage.h>

FsRoot_t *FsRoots;

uint8_t FsInit() {
  FsRoots = malloc(sizeof(FsRoot_t) * 2);

  // TODO Implement this
  FsRoots[0].root = malloc(2);
  strcpy(FsRoots[0].root, "/");
  FsRoots[0].fs_type = ROOT_TYPE_HYPNOTICFS;

  memset(&(FsRoots[1]), 0, sizeof(FsRoot_t));

  return 1;
}
