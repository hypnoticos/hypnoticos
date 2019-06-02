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
typedef struct _DispatcherFormatElfHeader_t DispatcherFormatElfHeader_t;
struct _DispatcherFormatElfHeader_t {
  uint8_t magic[4];
  uint8_t file_class;
  uint8_t bit_direction;
  uint8_t elf_version;
  uint8_t abi;
  uint8_t reserved[8];

  uint16_t type;
  uint16_t architecture;
  uint32_t version;

  uint64_t entry_point;

  uint64_t program_header_offset;
  uint64_t section_header_offset;

  uint32_t flags;
  uint16_t header_size;

  uint16_t program_header_entry_size;
  uint16_t program_header_entry_count;

  uint16_t section_header_entry_size;
  uint16_t section_header_entry_count;

  uint16_t section_header_names_entry;
};
struct _DispatcherFormatElfProgramHeader_t {
  uint32_t type;
  uint32_t flags;
  uint64_t offset;
  uint64_t va;
  uint64_t pa;
  uint64_t file_size;
  uint64_t memory_size;
  uint32_t align;
};

uint8_t DispatcherFormatElfDetect(char *data, uint64_t size);
uint8_t DispatcherFormatElfSetUp(DispatcherProcess_t *p);

#endif
