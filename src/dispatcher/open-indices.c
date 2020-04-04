//
// HypnoticOS
// Copyright (C) 2020  jk30
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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <hypnoticos/dispatcher.h>
#include <hypnoticos/lock.h>
#include <hypnoticos/hypnoticos.h>

uint8_t DispatcherOpenIndices_lock = LOCK_UNLOCKED;
DispatcherOpenIndex_t **DispatcherOpenIndices = NULL;

/**
 * Attempt to lock a path so that other processes cannot lock it.
 * @param  path The path.
 * @return      NULL on failure or the new entry on success.
 */
DispatcherOpenIndex_t *DispatcherIndexLockAttempt(DispatcherProcess_t *p, const char *path) {
  uint64_t index;

  // Lock DispatcherOpenIndices
  LockGet(&DispatcherOpenIndices_lock);

  // Try to find the path in DispatcherOpenIndices
  for(index = 0; DispatcherOpenIndices[index] != NULL; index++) {
    if(strcmp(DispatcherOpenIndices[index]->path, path) == 0) {
      WARNING();
      LockDone(&DispatcherOpenIndices_lock);
      return NULL;
    }
  }
  uint64_t max_entry = index;

  // Generate a new entry
  DispatcherOpenIndex_t *ptr;
  if((ptr = malloc(sizeof(DispatcherOpenIndex_t))) == NULL) {
    WARNING();
    LockDone(&DispatcherOpenIndices_lock);
    return NULL;
  }

  // Generate ID
  uint64_t id; // ID must not be 0
  uint8_t success = 0; // This checks if the main loop returns because OPEN_INDICES_MAX_ID is reached
  for(id = 1; id < OPEN_INDICES_MAX_ID; id++) {
    uint8_t fail = 0; // This only checks if the next loop fails

    for(index = 0; DispatcherOpenIndices[index] != NULL; index++) {
      if(DispatcherOpenIndices[index]->open_index_id == id) {
        fail = 1;
      }
    }

    if(!fail) {
      success = 1;
      break;
    }
  }

  // Was OPEN_INDICES_MAX_ID reached?
  if(!success) {
    WARNING();
    LockDone(&DispatcherOpenIndices_lock);
    free(ptr);
    return NULL;
  }

  if((ptr->path = malloc(strlen(path))) == NULL) {
    WARNING();
    LockDone(&DispatcherOpenIndices_lock);
    free(ptr);
    return NULL;
  }
  strcpy(ptr->path, path);
  ptr->process = p;
  ptr->directory_data = NULL;

  // Add the entry
  DispatcherOpenIndices = realloc(DispatcherOpenIndices, sizeof(DispatcherOpenIndex_t *) * (max_entry + 2));
  DispatcherOpenIndices[max_entry] = ptr;
  DispatcherOpenIndices[max_entry + 1] = NULL;

  // Unlock DispatcherOpenIndices
  LockDone(&DispatcherOpenIndices_lock);

  return ptr;
}

/**
 * Retrieve an index lock entry.
 * @param  id The entry id.
 * @return    NULL on failure, or the entry on success.
 */
DispatcherOpenIndex_t *DispatcherIndexLockRetrieve(DispatcherProcess_t *p, uint64_t id) {
  // Lock DispatcherOpenIndices
  LockGet(&DispatcherOpenIndices_lock);

  // Try to find the entry in DispatcherOpenIndices
  for(uint64_t index = 0; DispatcherOpenIndices[index] != NULL; index++) {
    if(DispatcherOpenIndices[index]->open_index_id == id && DispatcherOpenIndices[index]->process->pid == p->pid) {
      LockDone(&DispatcherOpenIndices_lock);
      return DispatcherOpenIndices[index];
    }
  }

  // Unlock DispatcherOpenIndices
  LockDone(&DispatcherOpenIndices_lock);

  return NULL;
}

/**
 * Unlock an index lock entry. This function assumes that directory_data is already free'd.
 * @param entry The entry.
 */
void DispatcherIndexLockDone(DispatcherOpenIndex_t *entry) {
  uint64_t index;

  // Lock DispatcherOpenIndices
  LockGet(&DispatcherOpenIndices_lock);

  // Move each entry forward after this entry
  uint8_t found = 0;
  for(index = 0; DispatcherOpenIndices[index] != NULL; index++) {
    if(!found && DispatcherOpenIndices[index]->open_index_id == entry->open_index_id) {
      found = 1;
    }

    if(found) {
      // Move the next entry here
      DispatcherOpenIndices[index] = DispatcherOpenIndices[index + 1];
    }
  }
  uint64_t max_entry = index;

  // Clean up
  free(entry->path);
  free(entry);

  // Remove the entry
  DispatcherOpenIndices = realloc(DispatcherOpenIndices, sizeof(DispatcherOpenIndex_t *) * (max_entry + 1));
  DispatcherOpenIndices[max_entry] = NULL;

  // Unlock DispatcherOpenIndices
  LockDone(&DispatcherOpenIndices_lock);
}
