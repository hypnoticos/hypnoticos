/*

HypnoticOS
Copyright (C) 2019, 2020  jk30

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

SECTIONS {
  ENTRY(_start)
  . = 0x10000000;
  .text : ALIGN(4K) {
    *(.text)
  }

  . = 0x10010000;
  .data : ALIGN(4K) {
    *(.data)
  }

  . = 0x10020000;
  .rodata : ALIGN(4K) {
    *(.rodata)
  }

  .bss : ALIGN(4K) {
    *(.bss)
  }
}
