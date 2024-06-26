//
// HypnoticOS
// Copyright (C) 2019, 2020, 2024  jk30
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
#include "../../include/hypnoticos/memory.h"

#define ERROR()       fprintf(stderr, "Error.\n");

int main(int argc, char *argv[]) {
  uint64_t offset, count, i, i2, i3;
  char *file, c;
  FILE *f;
  MemoryTableIndex_t mti;
  MemoryTable_t *mt, *mt_entry;

  fprintf(stdout, "HypnoticOS  Copyright (C) 2019 jk30\n");
  fprintf(stdout, "Memory dump tool v0.1\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "WARNING: The results of this tool should NOT be relied upon.\n");
  fprintf(stdout, "The results may differ from architecture to architecture so may be inaccurate.\n");
  fprintf(stdout, "\n");

  if(argc < 3 || argc > 4) {
    fprintf(stdout, "Usage: %s file address\n", argv[0]);
    fprintf(stdout, "  file      HypnoticOS memory dump file\n");
    fprintf(stdout, "  address   Hex address of MemoryTableIndices within the memory dump\n");
    fprintf(stdout, "  no-data   Optional - Set to 'y' to omit the data.\n");
    return 1;
  }

  short no_data = 0;
  if(argc == 4) {
    if(strcmp(argv[3], "y") != 0) {
      ERROR();
      return 1;
    }
    no_data = 1;
  }

  file = argv[1];
  if(sscanf(argv[2], "0x%lX", &offset) != 1) {
    ERROR();
    return 1;
  }

  if((f = fopen(file, "rb")) == NULL) {
    ERROR();
    return 1;
  }

  // Get first entry
  fseek(f, offset, SEEK_SET);
  if(fread(&mti, sizeof(MemoryTableIndex_t), 1, f) != 1) {
    ERROR();
    fclose(f);
    return 1;
  }

  while(1) {
    fprintf(stdout, "MTI at 0x%lX\n", offset);

    fprintf(stdout, "MT entries:\n");
    mt = malloc(mti.size);
    count = mti.size / 221;
    fseek(f, (uint64_t) mti.addr, SEEK_SET);
    if(fread(mt, sizeof(MemoryTable_t), count, f) != count) {
      ERROR();
      fclose(f);
      free(mt);
      return 1;
    }
    for(i = 0; i < count; i++) {
      mt_entry = (void *) ((uint64_t) mt + (i * sizeof(MemoryTable_t)));
      if(mt_entry->status == 0) {
        continue;
      }
      for(i2 = 0; mt_entry->function[i2] != 0; i2++) {
        if(mt_entry->function[i2] >= 'a' && mt_entry->function[i2] <= 'z') {
          continue;
        }
        if(mt_entry->function[i2] >= 'A' && mt_entry->function[i2] <= 'Z') {
          continue;
        }
        if(mt_entry->function[i2] >= '0' && mt_entry->function[i2] <= '9') {
          continue;
        }
        if(mt_entry->function[i2] == '_') {
          continue;
        }
        if(i2 == 0 && mt_entry->function[0] == '-' && mt_entry->function[1] == 0) {
          break;
        }

        fprintf(stderr, "ERROR: The next memory table entry doesn't look right. (possibly at physical address 0x%lX)\nStopped.\n", offset + (sizeof(MemoryTable_t) * i));
        free(mt);
        fclose(f);
        return 1;
      }
      fprintf(stdout, "addr=0x%lX\tsize=0x%lX\t%s:%u\n", mt_entry->addr, mt_entry->size, mt_entry->function, mt_entry->line);
      if(!no_data) {
        fprintf(stdout, "data:\n");

        fseek(f, (uint64_t) mt_entry->addr, SEEK_SET);
        for(i3 = 0; i3 < mt_entry->size; i3++) {
          if(fread(&c, 1, 1, f) != 1) {
            ERROR();
            fclose(f);
            free(mt);
            return 1;
          }

          if(i3 != 0 && (i3 % 16) == 0) {
            fprintf(stdout, "\n");
          }

          fprintf(stdout, "%02X ", c & 0xFF);
        }

        fprintf(stdout, "\n\n");
      }
    }
    fprintf(stdout, "\n");
    free(mt);

    // Is this the last entry?
    if(mti.next == NULL) {
      break;
    }

    // Get next entry
    offset = (uint64_t) mti.next;
    fseek(f, offset, SEEK_SET);
    if(fread(&mti, sizeof(MemoryTableIndex_t), 1, f) != 1) {
      ERROR();
      fclose(f);
      return 1;
    }
  }

  fclose(f);

  return 0;
}
