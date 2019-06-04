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

global TssBase_0_15, TssBase_16_23, TssBase_24_31, TssBase_32_63, GdtTemp, Gdt, GdtKernelCS_48_55

section .data
align 8
GdtTemp:
  dw (3 * 8) - 1
  dd GdtEntries

align 8
Gdt:
  dw (7 * 8) - 1
  dq GdtEntries

align 8
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
  GdtKernelCS_48_55 db 0xCF       ; Flags = 0xC, limit middle = 0xF
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
  db 0xA0       ; Flags = 0xA, limit middle = 0x0
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
  ; Limit = 0x2089 (size of TSS)
  ; Flags = 0x0
  ; TODO Update field names on extern/boot.nasm.
  ; Should this be 0x2089 or 0x2089+1 ? (another reference somewhere else too)
  dw 0x2089           ; TSS length (limit low)
  TssBase_0_15 dw 0   ; TSS address (load later)
  TssBase_16_23 db 0  ; TSS address (load later)
  db 0x89             ; 0b10001001
  db 0x00             ; Flags = 0x0, TSS length middle (limit middle) = 0x0
  TssBase_24_31 db 0  ; TSS address (load later)
  TssBase_32_63 dd 0  ; TSS address (load later)
  dd 0
