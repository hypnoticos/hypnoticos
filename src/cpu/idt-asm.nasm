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

global IdtSet
global Idt0, Idt1, Idt2, Idt3, Idt4, Idt5, Idt6, Idt7, Idt8, Idt9, Idt10, Idt11, Idt12, Idt13, Idt14, Idt16, Idt17, Idt18, Idt19, Idt20, Idt48, Idt49, Idt50, Idt51, Idt52, Idt53, Idt54, Idt55, Idt56, Idt57, Idt58, Idt59, Idt60, Idt61, Idt62, Idt63, Idt64, Idt65, Idt66, Idt67, Idt68, Idt69, Idt70, Idt71, Idt160, Idt240, Idt241, Idt242, IdtReserved, IdtCallVector, IdtCallSavedCr3, IdtCallSavedRsp, IdtCallSavedRbp, IdtCallSavedRax, IdtCallSavedRbx, IdtCallSavedRcx, IdtCallSavedRdx, IdtCallSavedRsi, IdtCallSavedRdi, IdtCallSavedRip, IdtCallSavedRflags, IdtCallSavedR8, IdtCallSavedR9, IdtCallSavedR10, IdtCallSavedR11, IdtCallSavedR12, IdtCallSavedR13, IdtCallSavedR14, IdtCallSavedR15, IdtStackTop, IdtLimit, IdtBase, IdtWait
extern IdtGates, IdtCall, MemoryKernelPML4, ApicLocalEoi, ApInitDone, ApicLocalSetUpTimer, ApStartNewStack, GdtEntries

section .data
align 8
  IdtCallSavedCr3 dq 0
  IdtCallSavedRsp dq 0
  IdtCallSavedRbp dq 0

  IdtCallSavedRax dq 0
  IdtCallSavedRbx dq 0
  IdtCallSavedRcx dq 0
  IdtCallSavedRdx dq 0
  IdtCallSavedRsi dq 0
  IdtCallSavedRdi dq 0
  IdtCallSavedRip dq 0
  IdtCallSavedRflags dq 0
  IdtCallSavedR8 dq 0
  IdtCallSavedR9 dq 0
  IdtCallSavedR10 dq 0
  IdtCallSavedR11 dq 0
  IdtCallSavedR12 dq 0
  IdtCallSavedR13 dq 0
  IdtCallSavedR14 dq 0
  IdtCallSavedR15 dq 0

  IdtCallVector dd 0
  IdtCallErrorCode db 0
  IdtBusy db 0

section .text
IdtSet:
  lidt [Idt]
  ret

IdtGetLock:
  push rax

  .GetLock:
    mov al, 1
    xchg [IdtBusy], al
    cmp al, 0
    je .LockObtained
    pause
    jmp .GetLock

  .LockObtained:
    pop rax
    ret

IdtWait:
  ; As the next interrupt to be called will not involve a switch from privilege level 3 to level 0, RSP will not be modified when the interrupt occurs. RSP needs to be reset to avoid the stack overflowing.
  mov rax, 0
  str ax
  add rax, GdtEntries

  ; First, find the TSS structure from the GDT entry
  ; First three bytes
  mov rdx, [rax]
  mov r8, rdx
  shr r8, 16
  and r8, 0xFFFFFF

  ; Fourth byte
  mov r9, rdx
  shr r9, 56
  and r9, 0xFF000000

  ; Final four bytes
  mov r10, [rax + 8]
  shl r10, 32

  ; Collate
  mov r11, 0
  or r11, r8
  or r11, r9
  or r11, r10

  ; Then find the stack address within the TSS structure
  mov rsp, [r11 + 4]

  call ApicLocalEoi
  mov byte [IdtBusy], 0
  sti
  hlt
  jmp $-1

IdtCallManage:
  push ax
  mov ax, 0x10
  mov ss, ax
  pop ax
  mov [IdtCallSavedRbp], rbp

  cmp byte [IdtCallErrorCode], 0
  jne .Continue

  push 0    ; False error code so that the stack is added to

  .Continue:
    push rax
    mov rax, [rsp + 16]
    mov [IdtCallSavedRip], rax
    mov rax, [rsp + 32]
    mov [IdtCallSavedRflags], rax
    mov rax, [rsp + 40]
    mov [IdtCallSavedRsp], rax
    pop rax

    mov [IdtCallSavedRax], rax
    mov [IdtCallSavedRbx], rbx
    mov [IdtCallSavedRcx], rcx
    mov [IdtCallSavedRdx], rdx
    mov [IdtCallSavedRsi], rsi
    mov [IdtCallSavedRdi], rdi
    mov [IdtCallSavedR8], r8
    mov [IdtCallSavedR9], r9
    mov [IdtCallSavedR10], r10
    mov [IdtCallSavedR11], r11
    mov [IdtCallSavedR12], r12
    mov [IdtCallSavedR13], r13
    mov [IdtCallSavedR14], r14
    mov [IdtCallSavedR15], r15

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rax, cr3
    mov [IdtCallSavedCr3], rax
    mov rax, MemoryKernelPML4
    mov cr3, rax

    call IdtCall
    call ApicLocalEoi

    mov rax, [IdtCallSavedCr3]
    mov cr3, rax

    mov ax, 0x20 | 0x3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rax, [IdtCallSavedRax]
    mov rbx, [IdtCallSavedRbx]
    mov rcx, [IdtCallSavedRcx]
    mov rdx, [IdtCallSavedRdx]
    mov rsi, [IdtCallSavedRsi]
    mov rdi, [IdtCallSavedRdi]
    mov r8, [IdtCallSavedR8]
    mov r9, [IdtCallSavedR9]
    mov r10, [IdtCallSavedR10]
    mov r11, [IdtCallSavedR11]
    mov r12, [IdtCallSavedR12]
    mov r13, [IdtCallSavedR13]
    mov r14, [IdtCallSavedR14]
    mov r15, [IdtCallSavedR15]

    mov rbp, [IdtCallSavedRbp]

    mov rsp, [IdtCallSavedRsp]

    push 0x23 ; SS
    push qword [IdtCallSavedRsp] ; RSP
    push qword [IdtCallSavedRflags] ; RFLAGS
    push 0x1b ; CS
    push qword [IdtCallSavedRip] ; RIP

    cmp byte [IdtCallErrorCode], 0
    je .SkipErrorCode

    push 0   ; An error code will be pop'd

  .SkipErrorCode:
    mov byte [IdtBusy], 0
    iretq

Idt0:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 0
  jmp IdtCallManage

Idt1:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 1
  jmp IdtCallManage

Idt2:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 2
  jmp IdtCallManage

Idt3:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 3
  jmp IdtCallManage

Idt4:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 4
  jmp IdtCallManage

Idt5:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 5
  jmp IdtCallManage

Idt6:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 6
  jmp IdtCallManage

Idt7:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 7
  jmp IdtCallManage

Idt8:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 8
  jmp IdtCallManage

Idt9:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 9
  jmp IdtCallManage

Idt10:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 10
  jmp IdtCallManage

Idt11:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 11
  jmp IdtCallManage

Idt12:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 12
  jmp IdtCallManage

Idt13:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 13
  jmp IdtCallManage

Idt14:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 14
  jmp IdtCallManage

Idt16:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 16
  jmp IdtCallManage

Idt17:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 17
  jmp IdtCallManage

Idt18:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 18
  jmp IdtCallManage

Idt19:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 19
  jmp IdtCallManage

Idt20:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 20
  jmp IdtCallManage

Idt48:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 48
  jmp IdtCallManage

Idt49:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 49
  jmp IdtCallManage

Idt50:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 50
  jmp IdtCallManage

Idt51:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 51
  jmp IdtCallManage

Idt52:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 52
  jmp IdtCallManage

Idt53:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 53
  jmp IdtCallManage

Idt54:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 54
  jmp IdtCallManage

Idt55:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 55
  jmp IdtCallManage

Idt56:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 56
  jmp IdtCallManage

Idt57:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 57
  jmp IdtCallManage

Idt58:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 58
  jmp IdtCallManage

Idt59:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 59
  jmp IdtCallManage

Idt60:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 60
  jmp IdtCallManage

Idt61:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 61
  jmp IdtCallManage

Idt62:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 62
  jmp IdtCallManage

Idt63:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 63
  jmp IdtCallManage

Idt64:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 64
  jmp IdtCallManage

Idt65:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 65
  jmp IdtCallManage

Idt66:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 26
  jmp IdtCallManage

Idt67:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 67
  jmp IdtCallManage

Idt68:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 68
  jmp IdtCallManage

Idt69:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 69
  jmp IdtCallManage

Idt70:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 70
  jmp IdtCallManage

Idt71:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 71
  jmp IdtCallManage

Idt160:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 160
  jmp IdtCallManage

Idt240:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 240
  jmp IdtCallManage

Idt241:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 241
  jmp IdtCallManage

Idt242:
  call ApStartNewStack
  mov rsp, rax
  mov rbp, rax

  call ApicLocalSetUpTimer
  call ApicLocalEoi

  mov byte [ApInitDone], 1
  sti ; Set IF
  mov byte [ApInitDone], 2

  hlt
  jmp $-1

IdtReserved:
  call IdtGetLock
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 15
  jmp IdtCallManage

section .data
align 4
Idt:
  IdtLimit dw 0         ; Limit
  IdtBase dq 0          ; Base address

section .bss
align 8
IdtStack:
  resb 8192
IdtStackTop:
