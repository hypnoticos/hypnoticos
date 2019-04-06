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

global Start, Stack
extern Main, GdtInit

section .text
Start:
  cli
  
  mov ebp, Stack
  mov esp, Stack

  push ebx      ; Multiboot struct
  push eax      ; Magic value

  ; Disable PIC interrupts
  mov al, 0xFF
  out byte 0xA1, al
  out byte 0x21, al

  ; Init GDT
  call GdtInit

  ; Enter protected mode (set PE bit [bit 0] of CR0)
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax

  call Main

  cli
Loop:
  hlt
  jmp Loop

section .bss
align 4         ; Must be 32-bit aligned
  resb 16384
Stack:
