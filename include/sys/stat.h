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

#ifndef SYS_STAT_H
#define SYS_STAT_H

#include <sys/types.h>
#include <stdint.h>

// mode_t
// 64-bits
// 0, 1, 2 = RWX for owner
// 3, 4, 5 = RWX for group
// 6, 7, 8 = RWX for everyone
// ...
// 20 = type is file
// 21 = type is directory

#define S_IRUSR           (0x1 << 0)
#define S_IWUSR           (0x1 << 1)
#define S_IXUSR           (0x1 << 2)
#define S_IRWXU           (S_IRUSR | S_IWUSR | S_IXUSR)

#define S_IRGRP           (0x1 << 3)
#define S_IWGRP           (0x1 << 4)
#define S_IXGRP           (0x1 << 5)
#define S_IRWXG           (S_IRGRP | S_IWGRP | S_IXGRP)

#define S_IROTH           (0x1 << 6)
#define S_IWOTH           (0x1 << 7)
#define S_IXOTH           (0x1 << 8)
#define S_IRWXO           (S_IROTH | S_IWOTH | S_IXOTH)

#define S_IFREG           (0x1 << 20)
#define S_IFDIR           (0x1 << 21)

struct stat {
  mode_t st_mode;
  uint64_t st_size;
};

int stat(const char *file, struct stat *dest);

#endif
