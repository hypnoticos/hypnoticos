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
#include <string.h>
#include <stdlib.h>
#include <hypnoticos/hypnoticos.h>

#define MAX_INPUT         80

void CommandRun(char *filename);

int main(int argc, char **argv) {
  char s[MAX_INPUT + 1], c;

  while(1) {
    memset(s, 0, MAX_INPUT + 1);
    printf("# ");

    // TODO malloc
    while(strlen(s) < MAX_INPUT) {
      c = fgetc(stdin);

      if(c == '\n') {
        break;
      } else if(c == '\b') {
        if(strlen(s) != 0) {
          s[strlen(s) - 1] = 0;
        }
      } else {
        s[strlen(s)] = c;
        putchar(c);
      }
    }
    putchar('\n');

    if(strcmp(s, "help") == 0) {
      puts("HypnoticOS Console");
      puts("There are no other built-in commands.");
    } else {
      CommandRun(s);
    }
    putchar('\n');
  }
}

void CommandRun(char *filename) {
  char c;

  if(filename[0] == 0) {
    return;
  }

  char **argv = NULL;
  char this_param[MAX_INPUT + 1];
  int param_count = 1;
  while(1) {
    memset(this_param, 0, MAX_INPUT + 1);
    printf("Parameter %u: ", param_count);

    // TODO malloc
    while(strlen(this_param) < MAX_INPUT) {
      c = fgetc(stdin);

      if(c == '\n') {
        break;
      } else if(c == '\b') {
        if(strlen(this_param) != 0) {
          this_param[strlen(this_param) - 1] = 0;
        }
      } else {
        this_param[strlen(this_param)] = c;
        putchar(c);
      }
    }
    putchar('\n');

    if(this_param[0] == 0) {
      break;
    } else {
      if(argv == NULL) {
        argv = malloc(sizeof(char *) * param_count);
      } else {
        argv = realloc(argv, sizeof(char *) * param_count);
      }

      argv[param_count - 1] = malloc(strlen(this_param) + 1);
      strcpy(argv[param_count - 1], this_param);
      param_count++;
    }
  }

  if(Run(filename, argv, param_count) == 0) {
    printf("Failed to run binary.\n");
  }
}
