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

#ifndef HYPNOTICOS_BOOT_H
#define HYPNOTICOS_BOOT_H

#include <multiboot.h>
#include <stdint.h>

extern uint32_t BootModulesCount, BootModulesAddr;
extern uint16_t TssLast;

uint8_t BootLoadModules();
void MultibootCheck(uint32_t magic, multiboot_info_t *multiboot);
uint8_t TssNew();

#endif
