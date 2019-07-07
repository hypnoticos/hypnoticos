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

#ifndef FCNTL_H
#define FCNTL_H

#include <sys/types.h>

#define O_RDONLY                    0x1
#define O_WRONLY                    (0x1 << 1)
#define O_RDWR                      (O_RDONLY | O_WRONLY)

int open(char *filename, int flags, mode_t mode);

#endif
