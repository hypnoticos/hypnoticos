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

typedef struct _MemoryBlock_t MemoryBlock_t;
struct _MemoryBlock_t {
  uint32_t start; /*!< The start of the memory block */
  uint32_t length; /*!< The length of the memory block */
  uint8_t type; /*!< The memory block's type */

  MemoryBlock_t *prev; /*!< Previous in the linked list (NULL if this is the first entry) */
  MemoryBlock_t *next; /*!< Next in the linked list (NULL if no more entries) */
};

/*!< A linked list containing the known available memory blocks */
MemoryBlock_t MemoryBlocks = {.start=0, .length=0, .type=0, .prev=NULL, .next=NULL};

void MemoryNewBlock(uint32_t start, uint32_t length, uint8_t type) {
  MemoryBlock_t *current;

  for(current = &MemoryBlocks; current->next == NULL; current = current->next);

  if(current == &MemoryBlocks) {
    // There are no other entries in the list
    current->start = start;
    current->length = length;
    current->type = type;
  } else {
    // TODO
  }
}
