//
// HypnoticFS Tools
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
#include <stdint.h>
#include <string.h>
#include "hypnoticfs.h"

int main(int argc, char *argv[]) {
  char *filename;
  uint64_t offset, index_size, size;
  FILE *f;

  if(argc != 3) {
    fprintf(stdout, "HypnoticOS  Copyright (C) 2019 jk30\n");
    fprintf(stdout, "HypnoticFS image creation tool v0.1\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "This tool will create an HypnoticFS image.\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Usage: %s file size\n", argv[0]);
    fprintf(stdout, "  file        Output file\n");
    fprintf(stdout, "  size        Output file size in MB (20 GB is the maximum)\n");
    return 1;
  }

  filename = argv[1];
  if(sscanf(argv[2], "%lu", &size) != 1) {
    ERROR("Invalid size.");
    return 1;
  }

  if(size > 1024 * 20) {
    ERROR("Size more than 20GB.");
    return 1;
  }
  size = size * 1024 * 1024;

  f = fopen(filename, "wb");
  if(f == NULL) {
    ERROR("Couldn't open file.");
    return 1;
  }

  // Write the header
  fprintf(f, "%cHypnoticFS", 0xFF); // Magic number
  index_size = size * 0.05;
  fwrite(&index_size, sizeof(uint64_t), 1, f); // Border between index and data
  offset = sizeof(uint64_t) + strlen("HypnoticFS") + 1;

  // Fill the remainder of the file
  for(; offset < size; offset++) {
    fputc(0, f);
  }

  fclose(f);

  return 0;
}
