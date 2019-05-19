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

global GdtInit, TssBaseLow, TssBaseMiddle, TssBaseHigh, Gdt

GdtInit:
  lgdt [Gdt]    ; Load the GDT
  mov ax, 0x10  ; GDT entry 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:GdtRet  ; GDT entry 0x08

GdtRet:
  ret

Gdt:
  dw (6 * 8) - 1
  dd GdtEntries

GdtEntries:
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

  ; TODO Restrict base/limit to user space
  ; User code segment - 0x18
  ; Base = 0x0
  ; Limit = 0xFFFFF (with granularity so that this spans the full 4GB address space)
  ; Flags = 0b1100 (0xC)
  dw 0xFFFF     ; Limit low
  dw 0          ; Base low
  db 0          ; Base middle
  db 0xFA       ; 0b11111010
  db 0xCF       ; Flags = 0xC, limit middle = 0xF
  db 0          ; Base high

  ; TODO Restrict base/limit to user space
  ; User data segment - 0x20
  ; Base = 0x0
  ; Limit = 0xFFFFF (with granularity so that this spans the full 4GB address space)
  ; Flags = 0b1100 (0xC)
  dw 0xFFFF     ; Limit low
  dw 0          ; Base low
  db 0          ; Base middle
  db 0xF2       ; 0b11110010
  db 0xCF       ; Flags = 0xC, limit middle = 0xF
  db 0          ; Base high

  ; TSS - 0x28
  ; Base = &Tss
  ; Limit = 0x20AA (size of TSS)
  ; Flags = 0x4
  dw 0x20AA                 ; TSS length (limit low)
  TssBaseLow dw 0           ; TSS address low (base low, load later)
  TssBaseMiddle db 0        ; TSS address middle (base middle, load later)
  db 0x89                   ; 0b10001001
  db 0x40                   ; Flags = 0x4, TSS length middle (limit middle) = 0x0
  TssBaseHigh db 0          ; TSS address high (base high, load later)
