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
#include <hypnoticos/function-codes.h>
#include <hypnoticos/function.h>

uint32_t KernelFunctionWrite(DispatcherProcess_t *p, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi) {
  uint32_t i;
  char *pa;

  // Check output buffer
  if(eax != 1) {
    return 0;
  }

  // Translate va to pa
  pa = GET_PA(ebx);
  if(pa == NULL) {
    return 0;
  }

  // Send to output buffer
  for(i = 0; i < ecx; i++) {
    fputc(pa[i], stdout);
  }

  return 0;
}
