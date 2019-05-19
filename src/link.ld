/*

HypnoticOS
Copyright (C) 2019  jk30

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

ENTRY(Start)

MEMORY {
  multiboot_header : ORIGIN = 0x500, LENGTH = 0x500
  ap_start_code : ORIGIN = 0x1000, LENGTH = 0x1000
  other : ORIGIN = 1M, LENGTH = 4M
}

SECTIONS {
  multiboot_header.rodata 0x500 : {
    boot/multiboot.o
  } >multiboot_header

  ap_start_code 0x1000 : ALIGN(4K) {
    /* .text must go first */
    cpu/ap-start-prepare.o(.text)
    cpu/ap-start-prepare.o(.data)
    cpu/ap-start-prepare.o(.bss)
  } >ap_start_code

  . = 1M;
  AddrStart = .;

  .text : ALIGN(4K) {
    *(.text)
  } >other

  .data : ALIGN(4K) {
    *(.data)
  } >other

  .rodata : ALIGN(4K) {
    *(.rodata)
  } >other

  .bss : ALIGN(4K) {
    *(.bss)
  } >other

  AddrEnd = .;
}
