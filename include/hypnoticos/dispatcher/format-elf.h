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

#ifndef HYPNOTICOS_DISPATCHER_FORMAT_ELF_H
#define HYPNOTICOS_DISPATCHER_FORMAT_ELF_H

#include <hypnoticos/dispatcher.h>
#include <stdint.h>

typedef struct _DispatcherFormatElfProgramHeader_t DispatcherFormatElfProgramHeader_t;
struct _DispatcherFormatElfProgramHeader_t {
  uint32_t type;
  uint32_t offset;
  uint32_t va;
  uint32_t pa;
  uint32_t file_size;
  uint32_t memory_size;
  uint32_t flags;
  uint32_t align;
};

uint8_t DispatcherFormatElfDetect(char *data, uint32_t size);
uint8_t DispatcherFormatElfSetUp(DispatcherProcess_t *p);

#endif
