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

; This code is to be run in real mode

bits 16

extern ApStart

section .text
ApStartPrepare:
  cli

  ; Stack
  mov esp, ApStartStack
  mov ebp, ApStartStack

  ; Clear all segments
  mov ax, 0
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  jmp 0x00:ApStartGdtInit

ApStartGdtInit:
  ; Set up a temporary GDT
  lgdt [ApStartGdt]

  ; Enter protected mode (set PE bit [bit 0] of CR0)
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  jmp 0x08:ApStartCall

bits 32
ApStartCall:
  call ApStart

ApStartLoop:
  hlt
  jmp ApStartLoop

section .data
align 4
ApStartGdt:
  dw (3 * 8) - 1
  dd ApStartGdtEntries

ApStartGdtEntries:
  ; NULL - 0x00
  dw 0          ; Limit low
  dw 0          ; Base low
  db 0          ; Base middle
  db 0          ; Access
  db 0          ; Flags and limit middle
  db 0          ; Base high

  ; Kernel code segment (CS) - 0x08
  ; Base = 0x0
  ; Limit = 0xFFFFF (with granularity so that this spans the full 4GB address space)
  ; Flags = 0b1100 (0xC)
  dw 0xFFFF     ; Limit low
  dw 0          ; Base low
  db 0          ; Base middle
  db 0x9A       ; 0b10011010
  db 0xCF       ; Flags = 0xC, limit middle = 0xF
  db 0          ; Base high

  ; Kernel data segment (DS) - 0x10
  ; Base = 0x0
  ; Limit = 0xFFFFF (with granularity so that this spans the full 4GB address space)
  ; Flags = 0b1100 (0xC)
  dw 0xFFFF     ; Limit low
  dw 0          ; Base low
  db 0          ; Base middle
  db 0x92       ; 0b10010010
  db 0xCF       ; Flags = 0xC, limit middle = 0xF
  db 0          ; Base high

section .bss
align 4
  resb 152
ApStartStack:
