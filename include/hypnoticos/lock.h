//
// HypnoticOS
// Copyright (C) 2020  jk30
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

#ifndef HYPNOTICOS_LOCK_H
#define HYPNOTICOS_LOCK_H

#include <stdint.h>

#define LOCK_UNLOCKED                   0

/**
 * Pause until the lock is available
 * @param token The lock.
 */
void LockGet(uint8_t *token);

/**
 * Allow another CPU to access this lock.
 * @param token The lock.
 */
void LockDone(uint8_t *token);

#endif
