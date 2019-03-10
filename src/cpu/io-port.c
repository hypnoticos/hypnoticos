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

#include <hypnoticos/cpu.h>

uint8_t IoPort8In(uint16_t port) {
  uint8_t data;

  asm("inb %1, %0"
    : "=a" (data)
    : "Nd" (port));

  return data;
}

void IoPort8Out(uint16_t port, uint8_t data) {
  asm("outb %0, %1"
    :
    : "a" (data), "Nd" (port));
}

uint16_t IoPort16In(uint16_t port) {
  uint16_t data;

  asm("inw %1, %0"
    : "=a" (data)
    : "Nd" (port));

  return data;
}

void IoPort16Out(uint16_t port, uint16_t data) {
  asm("outw %0, %1"
    :
    : "a" (data), "Nd" (port));
}

uint32_t IoPort32In(uint16_t port) {
  uint32_t data;

  asm("inl %1, %0"
    : "=a" (data)
    : "Nd" (port));

  return data;
}

void IoPort32Out(uint16_t port, uint32_t data) {
  asm("outl %0, %1"
    :
    : "a" (data), "Nd" (port));
}
