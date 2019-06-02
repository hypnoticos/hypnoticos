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


inline char *DispatcherFormatElfRead(DispatcherProcess_t *p, uint64_t start, uint64_t size);

uint8_t DispatcherFormatElfDetect(char *data, uint64_t size) {
  DispatcherFormatElfHeader_t *hdr;

  hdr = (DispatcherFormatElfHeader_t *) data;

  if(size <= 64) {
    return DISPATCHER_DETECT_FORMAT_NOT_DETECTED;
  }

  if(hdr->magic[0] != 0x7F && hdr->magic[1] != 'E' && hdr->magic[2] != 'L' && hdr->magic[3] != 'F') {
    return DISPATCHER_DETECT_FORMAT_NOT_DETECTED;
  }

  if(hdr->file_class != 2) {
    // Not 64-bit
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  }

  if(hdr->bit_direction != 1) {
    // Not little endian
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  }

  if(hdr->elf_version != 1) {
    // Version not 1
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  }

  if(hdr->abi != 0) {
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(hdr->type != 2) {
    // Not an executable
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(hdr->architecture != 0x3E) {
    // Not x86
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(hdr->version != 1) {
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  } else if(hdr->header_size < 64) {
    return DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED;
  }

  // TODO Flags

  return DISPATCHER_DETECT_FORMAT_DETECTED;
}

inline char *DispatcherFormatElfRead(DispatcherProcess_t *p, uint64_t start, uint64_t size) {
  if(p->data == NULL) {
    HALT();
  } else if(start + size >= p->size) {
    HALT();
  } else {
    return p->data + start;
  }
}

uint8_t DispatcherFormatElfSetUp(DispatcherProcess_t *p) {
  uint64_t offset, program_header_max;
  DispatcherFormatElfHeader_t *elf;
  DispatcherFormatElfProgramHeader_t *program_header;
  void *ptr;

  elf = (DispatcherFormatElfHeader_t *) p->data;

  // Set RIP
  DispatcherProcessSetRip(p, elf->entry_point);

  if(elf->program_header_offset == 0) {
    // No program header table
    HALT();
  }

  program_header_max = elf->program_header_offset + ((uint64_t) elf->program_header_entry_size * (uint64_t) elf->program_header_entry_count);
  for(offset = elf->program_header_offset; offset < program_header_max; offset = offset + (uint32_t) elf->program_header_entry_size) {
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
