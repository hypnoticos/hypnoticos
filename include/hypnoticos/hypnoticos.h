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

#ifndef HYPNOTICOS_HYPNOTICOS_H
#define HYPNOTICOS_HYPNOTICOS_H

#ifndef _HYPNOTICOS
#define _HYPNOTICOS       "Unknown"
#endif // _HYPNOTICOS

#ifdef _HYPNOTICOS_KERNEL
#define HALT()         printf("\nKERNEL HALTED (%s at line %u - in %s).", __FILE__, __LINE__, __FUNCTION__); \
                        while(1) { \
                          asm("hlt"); \
                        }

extern void *AddrStart, *AddrEnd;
#endif

#endif
