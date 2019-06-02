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
extern Main, Tss, TssBase_0_15, TssBase_16_23, TssBase_24_31, TssBase_32_63, MemoryKernelPML4, MemoryKernelPDPTE_0, MemoryKernelPDE_0, GdtKernelCS_48_55, GdtTemp, Gdt

section .text
bits 32
Start:
  cli

  mov ebp, Stack
  mov esp, Stack

  push dword 0  ; Pad to 8 bytes
  push ebx      ; Multiboot struct
  push dword 0  ; Pad to 8 bytes
  push eax      ; Magic value

  ; Disable PIC interrupts
  mov al, 0xFF
  out byte 0xA1, al
  out byte 0x21, al

  ; Init GDT
  lgdt [GdtTemp]    ; Load the GDT
  mov ax, 0x10  ; GDT entry 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:.GdtDone_1  ; GDT entry 0x08

  .GdtDone_1:

  ; Enter protected mode (set PE bit [bit 0] of CR0)
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax

  ; Enable PAE
  mov eax, cr4
  or eax, 0x20
  mov cr4, eax

  ; Load PML4
  mov eax, MemoryKernelPML4
  mov cr3, eax

  ; Set up first entry in the PML4
  mov dword [MemoryKernelPML4 + 4], 0
  mov eax, MemoryKernelPDPTE_0
  or eax, 0x3
  mov dword [MemoryKernelPML4], eax

  ; Set up first PDPTE entry within the first PML4 entry
  mov dword [MemoryKernelPDPTE_0 + 4], 0
  mov eax, MemoryKernelPDE_0
  or eax, 0x3
  mov dword [MemoryKernelPDPTE_0], eax

  ; Set up PDE entries in PDPTE entry 0
  mov ecx, 512
  .PDE_Loop:
    dec ecx

    mov eax, 0x200000
    mul ecx
    or eax, 0x83
    mov ebx, eax

    ; Add the entry
    mov eax, 8
    mul ecx
    add eax, MemoryKernelPDE_0
    mov [eax], ebx
    add eax, 4
    mov dword [eax], 0

    cmp ecx, 0
    jnz .PDE_Loop
  ; Set IA32_EFER.LME = 1
  ; C000_0080H
  ; Bit 8
  mov ecx, 0xC0000080
  rdmsr
  or eax, (0x1 << 8)
  wrmsr

  ; Enable paging
  mov eax, cr0
  or eax, 0x80000000        ; Set PG bit (bit 31)
  mov cr0, eax

  ;;; Now in IA-32e Mode

  ; Update kernel code segment
  mov byte [GdtKernelCS_48_55], 0xA0
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

  ; Update the TSS entry in the GDT
  mov eax, Tss
  mov word [TssBase_0_15], ax
  shr eax, 16
  mov byte [TssBase_16_23], al
  shr eax, 8
  mov byte [TssBase_24_31], al
  mov eax, Tss + 4
  mov dword [TssBase_32_63], eax

  ; Load task register
  mov ax, 0x28 ; TODO TSS is expanded to 16 bytes
  ltr ax

  pop rdi
  pop rsi
  call Main

  cli
Loop:
  hlt
  jmp Loop

section .bss
align 16
  resb 16384
Stack:
