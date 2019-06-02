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

#include <stdint.h>
#include <string.h>

// TODO Return value
int strcmp(const char *str1, const char *str2) {
  size_t i;

  if(strlen(str1) != strlen(str2)) {
    return 1;
  }

  for(i = 0; i < strlen(str1); i++) {
    if(str1[i] != str2[i]) {
      return 1;
    }
  }

  return 0;
}
