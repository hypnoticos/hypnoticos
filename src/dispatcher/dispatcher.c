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
#include <stdlib.h>
#include <string.h>
#include <hypnoticos/cpu.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/hypnoticos.h>
#include <hypnoticos/memory.h>

// TODO Save all registers

uint16_t DispatcherCurrentPid = 0;
uint16_t last_pid = 0;
DispatcherProcess_t **DispatcherProcesses;

uint8_t inline DispatcherProcessCheckVa(DispatcherProcess_t *p, uint32_t va, uint8_t kernel_function_ignore);

DispatcherProcess_t *DispatcherFind(uint16_t pid) {
  uint32_t i;

  for(i = 0; DispatcherProcesses[i] != NULL; i++) {
    if(DispatcherProcesses[i]->pid == pid) {
      return DispatcherProcesses[i];
    }
  }

  return NULL;
}

void DispatcherSetUpNext() {
  DispatcherProcess_t *p;
  uint16_t next_pid;
  uint32_t i, byte_offset, bit_offset, bit_operation;

  if(DispatcherCurrentPid != 0) {
    if((p = DispatcherFind(DispatcherCurrentPid)) == NULL) {
      HALT();
    }

    // Do not save CR3
    p->save.esp = IdtCallSavedEsp;
    p->save.ebp = IdtCallSavedEbp;
    p->save.eip = IdtCallSavedEip;

    p->save.eax = IdtCallSavedEax;
    p->save.ebx = IdtCallSavedEbx;
    p->save.ecx = IdtCallSavedEcx;
    p->save.edx = IdtCallSavedEdx;
    p->save.esi = IdtCallSavedEsi;
    p->save.edi = IdtCallSavedEdi;

    p->save.eflags = IdtCallSavedEflags;

    // Reset I/O port bitmap
    for(i = 0; i < p->io_count; i++) {
      byte_offset = p->io[i] / 8;
      Tss.io_permission[byte_offset] = 0xFF;
    }
  }

  // Find next process
  next_pid = DispatcherCurrentPid + 1;
  i = 0;
  while(1) {
    if((p = DispatcherFind(next_pid)) != NULL && p->run == 1) {
      break;
    }

    if(next_pid == 0xFFFF) {
      next_pid = 1;
      i++;
    } else {
      next_pid++;
    }

    if(i == 2) {
      printf("No processes found.\n");
      HALT();
    }
  }

  DispatcherCurrentPid = next_pid;

  // Restore registers
  IdtCallSavedCr3 = p->save.cr3;

  IdtCallSavedEip = p->save.eip;

  IdtCallSavedEsp = p->save.esp;
  IdtCallSavedEbp = p->save.ebp;

  IdtCallSavedEax = p->save.eax;
  IdtCallSavedEbx = p->save.ebx;
  IdtCallSavedEcx = p->save.ecx;
  IdtCallSavedEdx = p->save.edx;
  IdtCallSavedEsi = p->save.esi;
  IdtCallSavedEdi = p->save.edi;

  // Set up I/O port bitmap
  for(i = 0; i < p->io_count; i++) {
    byte_offset = p->io[i] / 8;
    bit_offset = p->io[i] % 8;
    bit_operation = 0x1 << bit_offset;
    Tss.io_permission[byte_offset] = Tss.io_permission[byte_offset] ^ bit_operation;
  }
}

uint8_t DispatcherInit() {
  DispatcherProcesses = malloc(sizeof(DispatcherProcess_t *));
  DispatcherProcesses[0] = NULL;

  return 1;
}

uint8_t DispatcherProcessSetUpStack(DispatcherProcess_t *p, uint32_t size) {
  uint32_t alloc_size, i, count;

  alloc_size = size + (size % 4096);

  p->stack = malloc_align(alloc_size, ALIGN_4KB);
  if(p->stack == NULL) {
    WARNING();
    return 0;
  }
  memset(p->stack, 0, alloc_size);

  p->save.ebp = 0xFFFF0000 + alloc_size - 1;
  p->save.esp = p->save.ebp;

  count = (size / 4096) + 1;
  for(i = 0; i < count; i++) {
    if(!DispatcherProcessMap(p, 0xFFFF0000 + (i * 4096), (uint32_t) p->stack + (i * 4096), 0, PAGING_PRESENT | PAGING_RW | PAGING_USER)) {
      WARNING();
      return 0;
    }
  }

  return 1;
}

DispatcherProcess_t *DispatcherProcessNew(char *name) {
  DispatcherProcess_t *p;
  uint32_t i;

  p = malloc(sizeof(DispatcherProcess_t));
  memset(&p->save, 0, sizeof(DispatcherProcessSave_t));

  while(1) { // TODO Check if max processes reached
    if(last_pid == 0xFFFF) {
      last_pid = 1;
    } else {
      last_pid++;
    }

    if(DispatcherFind(last_pid) == NULL) {
      p->pid = last_pid;
      break;
    }
  }

  p->stack = NULL;
  p->run = 0;

  p->save.cr3 = (uint32_t) MemoryPagingNewPD(); // TODO Parse this and note all allocated entries - mark them as mapped in p->va and set to ignore

  p->name = malloc(strlen(name) + 1);
  strcpy(p->name, name);

  p->va = malloc(sizeof(DispatcherProcessVa_t *));
  p->va[0] = NULL;

  p->alloc = malloc(sizeof(void *));
  p->alloc[0] = NULL;

  p->io = malloc(sizeof(uint16_t));
  p->io[0] = 0;
  p->io_count = 0;

  for(i = 0; DispatcherProcesses[i] != NULL; i++);

  DispatcherProcesses = realloc(DispatcherProcesses, sizeof(DispatcherProcess_t *) * (i + 2));
  DispatcherProcesses[i] = p;
  DispatcherProcesses[i + 1] = NULL;

  return p;
}

void DispatcherProcessAddIo(DispatcherProcess_t *p, uint16_t port) {
  p->io_count++;
  p->io = realloc(p->io, sizeof(uint16_t) * p->io_count);
  p->io[p->io_count - 1] = port;
}

void DispatcherProcessRun(DispatcherProcess_t *p) {
  p->run = 1;
}

void DispatcherProcessSetEip(DispatcherProcess_t *p, uint32_t eip) {
  p->save.eip = eip;
}

uint8_t inline DispatcherProcessCheckVa(DispatcherProcess_t *p, uint32_t va, uint8_t kernel_function_ignore) {
  uint32_t i;

  for(i = 0; p->va[i] != NULL; i++) {
    if(p->va[i]->va == va) {
      if(kernel_function_ignore == 1 && p->va[i]->ignore == 1) {
        // Found, but to be ignored
        return 2;
      } else {
        // Found
        return 1;
      }
    }
  }

  return 0;
}

uint8_t DispatcherProcessMap(DispatcherProcess_t *p, uint32_t va, uint32_t pa, uint8_t kernel_function_ignore, uint32_t flags) {
  uint32_t i;

  // Check if va and pa are 4KB aligned
  if((va & 0xFFF) || (pa & 0xFFF)) {
    WARNING();
    return 0;
  }

  // Check for duplicates in p->va
  i = DispatcherProcessCheckVa(p, va, kernel_function_ignore);
  if(i != 0) {
    WARNING();
    return 0;
  }

  // Add to p->va
  for(i = 0; p->va[i] != NULL; i++);
  p->va = realloc(p->va, sizeof(DispatcherProcessVa_t *) * (i + 2));
  p->va[i] = malloc(sizeof(DispatcherProcessVa_t));
  p->va[i + 1] = NULL;

  p->va[i]->ignore = kernel_function_ignore;
  p->va[i]->va = va;
  p->va[i]->pa = (void *) pa;

  return MemoryPagingSetPage((uint32_t *) p->save.cr3, va, pa, flags);
}

void *DispatcherProcessGetPa(DispatcherProcess_t *p, uint32_t va, uint8_t ignore) {
  uint32_t i, va_range, offset;

  va_range = va & 0xFFFFF000;
  offset = va & 0xFFF;

  for(i = 0; p->va[i] != NULL; i++) {
    if(p->va[i]->va == va_range) {
      if(ignore == 1 && p->va[i]->ignore == 1) {
        // Found, but to be ignored
        WARNING();
        return NULL;
      } else {
        // Found
        return p->va[i]->pa + offset;
      }
    }
  }

  // Not found
  WARNING();
  return NULL;
}

DispatcherProcess_t *DispatcherProcessNewFromFormat(char *name, char *data, uint32_t size) {
  uint32_t format = 0;
  DispatcherProcess_t *p;

  switch(DispatcherFormatElfDetect(data, size)) {
    case DISPATCHER_DETECT_FORMAT_DETECTED:
    format = DISPATCHER_FORMAT_ELF;
    break;

    case DISPATCHER_DETECT_FORMAT_DETECTED_UNSUPPORTED:
    WARNING();
    return NULL;

    case DISPATCHER_DETECT_FORMAT_NOT_DETECTED:
    WARNING();
    break;
  }

  if(format == 0) {
    WARNING();
    return NULL;
  }

  p = DispatcherProcessNew(name);
  if(p == NULL) {
    WARNING();
    return NULL;
  }
  p->data = data;
  p->size = size;

  if(format == DISPATCHER_FORMAT_ELF) {
    if(!DispatcherFormatElfSetUp(p)) {
      // TODO Clean up
      WARNING();
      return NULL;
    }
  } else {
    // TODO Clean up
    WARNING();
    return NULL;
  }

  // Check if EIP is set
  if(p->save.eip == 0) {
    // TODO Clean up
    WARNING();
    return NULL;
  }

  // Set up a stack
  if(!DispatcherProcessSetUpStack(p, 4096)) {
    // TODO Clean up
    WARNING();
    return NULL;
  }

  DispatcherProcessRun(p);

  return p;
}

void *DispatcherProcessAllocatePage(DispatcherProcess_t *p, uint32_t va, uint8_t kernel_function_ignore, uint32_t flags) {
  uint32_t i;
  void *ptr;

  ptr = malloc_align(4096, ALIGN_4KB);
  if(ptr == NULL) {
    WARNING();
    return NULL;
  }

  if(!DispatcherProcessMap(p, va, (uint32_t) ptr, kernel_function_ignore, flags)) {
    free(ptr);
    WARNING();
    return NULL;
  }

  for(i = 0; p->alloc[i] != NULL; i++);

  p->alloc = realloc(p->alloc, sizeof(void *) * (i + 2));
  p->alloc[i] = ptr;
  p->alloc[i + 1] = NULL;

  memset(ptr, 0, 4096);

  return ptr;
}

uint8_t DispatcherProcessLoadAt(DispatcherProcess_t *p, uint32_t va, char *data, uint32_t file_size, uint32_t memory_size, uint32_t flags) {
  uint32_t i, memory_count, file_count, page_min_addr, page_max_addr, pages, initial_offset;
  void **ptrs;

  memory_count = (memory_size / 4096) + 1;
  file_count = (file_size / 4096) + 1;
  if(memory_count < file_count) {
    WARNING();
    return 0;
  }

  page_min_addr = va & 0xFFFFF000;
  page_max_addr = (va + memory_size) | 0xFFF;
  pages = ((page_max_addr + 1) - page_min_addr) / 4096;

  ptrs = malloc(sizeof(void *) * pages);
  for(i = 0; i < pages; i++) {
    // TODO Check if page is already allocated.

    if((ptrs[i] = DispatcherProcessAllocatePage(p, page_min_addr + (i * 4096), 0, flags)) == NULL) {
      // TODO Clean up
      WARNING();
      return 0;
    }
  }

  if(file_size != 0) {
    initial_offset = va & 0xFFF;
    for(i = 0; i < file_count; i++) {
      // TODO 09/05/2019, start copying to va
      if(i == 0) {
        memcpy(ptrs[i] + initial_offset, data, 4096 - initial_offset);
      } else if(i != file_count - 1) {
        memcpy(ptrs[i], &data[(i * 4096) - (4096 - initial_offset)], 4096);
      } else {
        memcpy(ptrs[i], &data[(i * 4096) - (4096 - initial_offset)], file_size % 4096);
      }
    }
  }

  return 1;
}
