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
#include <unistd.h>
#include <hypnoticos/hypnoticos.h>

#define MAX_INPUT         80
#define CWD_MAX           100

void CommandRun(char *filename);
void CommandChangeDirectory();

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
      puts("Built-in commands:");
      puts(" cd             Change the working directory");
      puts(" help           Display this message");
    } else if(strcmp(s, "cd") == 0) {
      CommandChangeDirectory();
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

void CommandChangeDirectory() {
  char c, cwd[CWD_MAX + 1];

  if(getcwd(cwd, CWD_MAX) != cwd) {
    puts("Error.");
    return;
  }

  char new_directory[MAX_INPUT + 1];
  memset(new_directory, 0, MAX_INPUT + 1);
  printf("Current directory: %s\n", cwd);
  printf("New directory: ");

  // TODO malloc
  while(strlen(new_directory) < MAX_INPUT) {
    c = fgetc(stdin);

    if(c == '\n') {
      break;
    } else if(c == '\b') {
      if(strlen(new_directory) != 0) {
        new_directory[strlen(new_directory) - 1] = 0;
      }
    } else {
      new_directory[strlen(new_directory)] = c;
      putchar(c);
    }
  }
  putchar('\n');

  if(new_directory[0] == 0) {
    puts("Command cancelled.");
    return;
  }

  if(new_directory[0] != '/') {
    // This is a relative path
    uint16_t size = strlen(cwd) + 1 + strlen(new_directory) + 1;
    char *new_working_directory = malloc(size);
    snprintf(new_working_directory, size, "%s/%s", cwd, new_directory);
    if(chdir(new_working_directory) != 0) {
      puts("Error.");
    } else {
      printf("Directory changed to %s\n", new_working_directory);
    }
    free(new_working_directory);
  } else {
    if(chdir(new_directory) != 0) {
      puts("Error.");
    } else {
      printf("Directory changed to %s\n", new_directory);
    }
  }
}
