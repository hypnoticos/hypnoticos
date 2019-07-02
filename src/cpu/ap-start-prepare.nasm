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

extern MemoryKernelPML4, ApStart, Gdt, TssLast

section .text
bits 16
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
  mov word [ApStartGdt], (3 * 8) - 1
  mov dword [ApStartGdt + 2], ApStartGdtEntries

  ; Null
  mov dword [ApStartGdtNull_0_31], 0
  mov dword [ApStartGdtNull_32_63], 0

  ; CS
  mov dword [ApStartGdtCs_0_15], 0xFFFF
  mov dword [ApStartGdtCs_16_31], 0
  mov byte [ApStartGdtCs_32_39], 0
  mov byte [ApStartGdtCs_40_47], 0x9A
  mov byte [ApStartGdtCs_48_55], 0xCF
  mov byte [ApStartGdtCs_56_63], 0

  ; DS
  mov dword [ApStartGdtDs_0_15], 0xFFFF
  mov dword [ApStartGdtDs_16_31], 0
  mov byte [ApStartGdtDs_32_39], 0
  mov byte [ApStartGdtDs_40_47], 0x92
  mov byte [ApStartGdtDs_48_55], 0xCF
  mov byte [ApStartGdtDs_56_63], 0

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

  jmp 0x08:ApStart32Bit

bits 32
ApStart32Bit:
  ; Enable PAE
  mov eax, cr4
  or eax, 0x20
  mov cr4, eax

  ; Load PML4
  mov eax, MemoryKernelPML4
  mov cr3, eax

  ; Set IA32_EFER.LME = 1
  ; C000_0080H
  ; Bit 8
  mov ecx, 0xC0000080
  rdmsr
  or eax, (0x1 << 8)
  wrmsr

  ; Enable paging
  ; Set PG (bit 31) and WP (bit 16) bits
  mov eax, cr0
  or eax, 0x80010000
  mov cr0, eax

  ;;; Now in IA-32e Mode

  ; Update kernel code segment
  mov byte [ApStartGdtCs_48_55], 0xA0
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  jmp 0x08:.GdtDone_2

  .GdtDone_2:
  ;;; Now in 64-bit mode
bits 64
  lgdt [Gdt]

  ; Load task register
  ltr [TssLast]

  call ApStart

section .data
align 4
ApStartGdt:
  dw 0
  dd 0

align 4
ApStartGdtEntries:
  ; NULL - 0x00
  ApStartGdtNull_0_31  dd 0
  ApStartGdtNull_32_63 dd 0

  ; Kernel code segment (CS) - 0x08
  ApStartGdtCs_0_15 dw 0
  ApStartGdtCs_16_31 dw 0
  ApStartGdtCs_32_39 db 0
  ApStartGdtCs_40_47 db 0
  ApStartGdtCs_48_55 db 0
  ApStartGdtCs_56_63 db 0

  ; Kernel data segment (DS) - 0x10
  ApStartGdtDs_0_15 dw 0
  ApStartGdtDs_16_31 dw 0
  ApStartGdtDs_32_39 db 0
  ApStartGdtDs_40_47 db 0
  ApStartGdtDs_48_55 db 0
  ApStartGdtDs_56_63 db 0

section .bss
align 4
  resb 152
ApStartStack:
