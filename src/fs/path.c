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

#include <string.h>
#include <stdlib.h>
#include <hypnoticos/fs.h>

uint8_t PathConsolidate(const char *path, char *dest) {
  char *buffer, **segments;
  uint64_t i, c, i2, o, buffer_size;

  if(path[0] != '/') {
    return 0;
  } else if(strlen(path) == 1) {
    strcpy(dest, "/");
    return 1;
  } else if(strlen(path) > FS_MAX_PATH_SIZE) {
    return 0;
  }

  segments = malloc(sizeof(char *));

  buffer_size = strlen(path) + 1;
  buffer = malloc(buffer_size);
  memset(buffer, 0, buffer_size);

  c = 0;
  o = 0;
  for(i = 1; i <= strlen(path); i++) {
    if(i == strlen(path) || path[i] == '/') {
      buffer[o] = 0;

      segments = realloc(segments, sizeof(char *) * (c + 2));

      segments[c] = malloc(strlen(buffer) + 1);
      strcpy(segments[c], buffer);

      o = 0;
      c++;
    } else {
      buffer[o] = path[i];
      o++;
    }
  }

  if(c == 0) {
    free(segments);
    return 0;
  }
  for(i = 0; i < c; i++) {
    if(strcmp(segments[i], ".") == 0 || strcmp(segments[i], "") == 0) {
      free(segments[i]);
      segments[i] = NULL;
    } else if(strcmp(segments[i], "..") == 0) {
      free(segments[i]);
      segments[i] = NULL;
      if(i != 0 && segments[i - 1] != NULL) {
        free(segments[i - 1]);
        segments[i - 1] = NULL;
      }
    }
  }

  memset(dest, 0, FS_MAX_PATH_SIZE + 1);
  for(i = 0; i < c; i++) {
    if(segments[i] == NULL) {
      continue;
    }
    if((i2 = strlen(dest) + 1 + strlen(segments[i]) + 1) > FS_MAX_PATH_SIZE) {
      // TODO Memory clean up
      memset(dest, 0, FS_MAX_PATH_SIZE + 1);
      return 0;
    }
    dest[strlen(dest)] = '/';
    strcat(dest, segments[i]);
  }

  if(strcmp(dest, "") == 0) {
    dest[0] = '/';
    dest[1] = 0;
  }

  // TODO Memory clean up

  return 1;
}

char **PathBreakdown(const char *path) {
  char **r, *buffer, *short_path;
  uint64_t i, count, buffer_size;

  short_path = malloc(FS_MAX_PATH_SIZE + 1);
  if(PathConsolidate(path, short_path) != 1) {
    free(short_path);
    return NULL;
  }

  r = malloc(sizeof(char *));
  buffer_size = strlen(path) + 1;
  buffer = malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  count = 0;

  for(i = 0; i < strlen(short_path); i++) {
    if(short_path[i] == '/') {
      if(strcmp(buffer, "") == 0) {
        continue;
      }

      r = realloc(r, sizeof(char *) * (count + 2));
      r[count] = malloc(buffer_size);
      memcpy(r[count], buffer, buffer_size);
      count++;

      memset(buffer, 0, buffer_size);
    } else {
      buffer[strlen(buffer)] = short_path[i];
    }
  }

  if(strcmp(buffer, "") != 0) {
    r = realloc(r, sizeof(char *) * (count + 2));
    r[count] = malloc(buffer_size);
    memcpy(r[count], buffer, buffer_size);
    count++;
  }
  r[count] = NULL;
  free(buffer);

  return r;
}
