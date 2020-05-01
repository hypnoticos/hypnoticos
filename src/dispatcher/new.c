//
// HypnoticOS
// Copyright (C) 2019, 2020  jk30
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
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/fs.h>
#include <hypnoticos/memory.h>
#include <hypnoticos/hypnoticos.h>

uint8_t DispatcherProcessSetUpStack(DispatcherProcess_t *p, uint64_t size) {
  uint64_t alloc_size, i, count;

  alloc_size = size + (size % 4096);

  p->stack = malloc_align(alloc_size, ALIGN_4KB);
  if(p->stack == NULL) {
    WARNING();
    return 0;
  }
  memset(p->stack, 0, alloc_size);

  p->save.rbp = 0xFFFFFF0000 + alloc_size - 8;
  p->save.rsp = p->save.rbp;

  count = (size / 4096) + 1;
  for(i = 0; i < count; i++) {
    if(!DispatcherProcessMap(p, 0xFFFFFF0000 + (i * 4096), (uint64_t) p->stack + (i * 4096), 0, PAGING_PRESENT | PAGING_RW | PAGING_USER)) {
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
  p->heap_addr = 0x20000000;
  p->heap_size = 0x1000;
  p->run = 0;
  p->last_cycle = 0;
  p->lock = 0;
  p->suspend = 0;
  p->suspend_data = NULL;
  p->pending_exit = 0;

  p->save.cr3 = (uint64_t) MemoryPagingNewPD(); // TODO Parse this and note all allocated entries - mark them as mapped in p->va and set to ignore

  p->name = malloc(strlen(name) + 1);
  strcpy(p->name, name);

  p->va = malloc(sizeof(DispatcherProcessVa_t *));
  p->va[0] = NULL;

  p->alloc = malloc(sizeof(void *));
  p->alloc[0] = NULL;

  p->io = malloc(sizeof(uint16_t));
  p->io[0] = 0;
  p->io_count = 0;

  DispatcherProcessAllocatePage(p, p->heap_addr, 0, PAGING_USER | PAGING_RW | PAGING_PRESENT);

  if(!DispatcherProcessSetUpInitData(p)) {
    return NULL;
  }

  char **argv = malloc(sizeof(char *));
  int argc = 1;
  if((argv[0] = DispatcherProcessAddInitData(p, p->name, strlen(p->name) + 1)) == NULL) {
    return NULL;
  }

  uint64_t argv_addr;
  if((argv_addr = (uint64_t) DispatcherProcessAddInitData(p, argv, (argc * sizeof(char *)))) == NULL) {
    return NULL;
  }

  free(argv);

  p->save.rdi = argc;
  p->save.rsi = (uint64_t) argv_addr;

  for(i = 0; DispatcherProcesses[i] != NULL; i++);

  DispatcherProcesses = realloc(DispatcherProcesses, sizeof(DispatcherProcess_t *) * (i + 2));
  DispatcherProcesses[i] = p;
  DispatcherProcesses[i + 1] = NULL;

  return p;
}

#ifndef _HYPNOTICOS_TESTS

DispatcherProcess_t *DispatcherProcessNewFromFormat(char *path) {
  uint32_t format = 0;
  DispatcherProcess_t *p;
  FsIndex_t *file_index;
  char *data;

  // Read file
  // Get FS entry details
  if((file_index = FsDetailsGet(path)) == NULL) {
    WARNING();
    return NULL;
  }

  // Entry must be INDEX_TYPE_FILE
  if(file_index->type != INDEX_TYPE_FILE) {
    WARNING();
    return NULL;
  }

  // Allocate memory for data
  if((data = malloc(file_index->size)) == NULL) {
    WARNING();
    return NULL;
  }

  // Read data
  if(FsRead(path, 0, file_index->size, (uint8_t *) data) != file_index->size) {
    free(data);
    WARNING();
    return NULL;
  }

  switch(DispatcherFormatElfDetect(data, file_index->size)) {
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
    free(data);
    return NULL;
  }

  p = DispatcherProcessNew(path);
  if(p == NULL) {
    WARNING();
    free(data);
    return NULL;
  }
  p->data = data;
  p->size = file_index->size;

  if(format == DISPATCHER_FORMAT_ELF) {
    if(!DispatcherFormatElfSetUp(p)) {
      // TODO Clean up
      WARNING();
      free(data);
      return NULL;
    }
  } else {
    // TODO Clean up
    WARNING();
    free(data);
    return NULL;
  }

  // Check if RIP is set
  if(p->save.rip == 0) {
    // TODO Clean up
    WARNING();
    free(data);
    return NULL;
  }

  // Set up a stack
  if(!DispatcherProcessSetUpStack(p, 4096)) {
    // TODO Clean up
    WARNING();
    free(data);
    return NULL;
  }

  DispatcherProcessRun(p);

  return p;
}

#endif
