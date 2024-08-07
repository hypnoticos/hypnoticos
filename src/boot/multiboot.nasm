;
; HypnoticOS
; Copyright (C) 2019  jk30
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

align 4
section .rodata
; The multiboot header to be read by GRUB.
MultibootHeader:
  dd 0x1BADB002                   ; Magic value
  dd 0x00000002                   ; Flags: MULTIBOOT_MEMORY_INFO
  dd -(0x1BADB002 + 0x00000002)   ; Checksum
