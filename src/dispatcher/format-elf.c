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

#include <stdio.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/dispatcher/format-elf.h>
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>

#define PROGRAM_HEADER_LOAD               0x1

inline char *DispatcherFormatElfRead(DispatcherProcess_t *p, uint32_t start, uint32_t size);

uint8_t DispatcherFormatElfDetect(char *data, uint32_t size) {
  if(size <= 54) {
    return DISPATCHER_DETECT_FORMAT_NOT_DETECTED;
  } else if(data[0] != 0x7F && data[1] != 'E' && data[2] != 'L' && data[3] != 'F') {
    return DISPATCHER_DETECT_FORMAT_NOT_DETECTED;
  } else if(data[4] != 1) {
    // Not 32-bit
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(data[5] != 1) {
    // Not little endian
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(data[6] != 1) {
    // Version not 1
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(data[7] != 0) {
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(*((uint16_t *) &(data[16])) != 2) {
    // Not an executable
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(*((uint16_t *) &(data[18])) != 3) {
    // Not x86
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(*((uint32_t *) &(data[20])) != 1) {
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(*((uint16_t *) &(data[40])) < 52) {
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  }

  // TODO Flags

  return DISPATCHER_DETECT_FORMAT_DETECTED;
}

inline char *DispatcherFormatElfRead(DispatcherProcess_t *p, uint32_t start, uint32_t size) {
  if(p->data == NULL) {
    HALT();
  } else if(start + size >= p->size) {
    HALT();
  } else {
    return p->data + start;
  }
}

uint8_t DispatcherFormatElfSetUp(DispatcherProcess_t *p) {
  uint32_t program_header_addr, offset, program_header_max;
  uint16_t program_header_entry_size, program_header_entry_count;
  DispatcherFormatElfProgramHeader_t *program_header;
  void *ptr;

  // Set EIP
  DispatcherProcessSetEip(p, *((uint32_t *) &(p->data[24])));

  // Parse the program headers
  program_header_addr = *((uint32_t *) DispatcherFormatElfRead(p, 28, sizeof(uint32_t)));
  program_header_entry_size = *((uint16_t *) DispatcherFormatElfRead(p, 42, sizeof(uint16_t)));
  program_header_entry_count = *((uint16_t *) DispatcherFormatElfRead(p, 44, sizeof(uint16_t)));

  if(program_header_addr == 0) {
    // No program header table
    HALT();
  }

  program_header_max = program_header_addr + ((uint32_t) program_header_entry_size * (uint32_t) program_header_entry_count);
  for(offset = program_header_addr; offset < program_header_max; offset = offset + (uint32_t) program_header_entry_size) {
    program_header = (DispatcherFormatElfProgramHeader_t *) DispatcherFormatElfRead(p, offset, sizeof(DispatcherFormatElfProgramHeader_t));

    if(program_header->align != 0x1000) {
      HALT();
    }

    switch(program_header->type) {
      case PROGRAM_HEADER_LOAD:
      ptr = NULL;
      if(program_header->file_size != 0 && (ptr = DispatcherFormatElfRead(p, program_header->offset, program_header->file_size)) == NULL) {
        HALT();
      }
      if(!DispatcherProcessLoadAt(p, program_header->va, (char *) ptr, program_header->file_size, program_header->memory_size, PAGING_PRESENT | PAGING_RW | PAGING_USER)) { // TODO Flags
        // TODO Clean up
        WARNING();
        return 0;
      }
      break;

      default:
      // TODO
      HALT();
      break;
    }
  }
  if(offset != program_header_max) {
    // TODO Clean up
    WARNING();
    return 0;
  }

  return 1;
}
