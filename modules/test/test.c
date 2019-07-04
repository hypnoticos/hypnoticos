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
#include <unistd.h>

int main(int argc, char **argv) {
  char s[80], c;

  while(1) {
    memset(s, 0, 80);
    puts("Input: ");

    while(strlen(s) < 79) {
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

    puts("\n\nOutput: ");
    puts(s);
    puts("");
  }
}
