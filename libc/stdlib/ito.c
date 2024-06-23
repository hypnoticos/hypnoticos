//
// HypnoticOS
// Copyright (C) 2024  jk30
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
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#define BASE_ITOA       10
#define BASE_ITOH       16

void _ito(int64_t i, char *a, short base);

void itoa(int64_t i, char *a) {
    _ito(i, a, BASE_ITOA);
}

void itoh(int64_t i, char *a) {
    _ito(i, a, BASE_ITOH);
}

void _ito(int64_t i, char *a, short base) {
    char r[HYPNOTICOS_MAX_DIGITS + 2]; // Space for NUL and for a minus sign
    
    if(i == 0) {
        strcpy(a, "0");
        return;
    }
    
    short negative = 0;
    if(i < 0) {
        negative = 1;
    }

    memset(r, 0, HYPNOTICOS_MAX_DIGITS + 2);

    int it = HYPNOTICOS_MAX_DIGITS + 1;
    while(it > 1 && i != 0) {
        int t = i % base;
        if(base == BASE_ITOA) {
            r[it] = '0' + t;
        } else {
            if(t < 0xA) {
                r[it] = '0' + t;
            } else {
                r[it] = 'A' + (t - 10);
            }
        }
        i = (i - t) / base;
        
        it--;
    }
    r[HYPNOTICOS_MAX_DIGITS + 2] = 0;

    if(negative) {
        r[it] = '-';
    } else {
        it++;
    }

    strcpy(a, r + it);
    return;
}
