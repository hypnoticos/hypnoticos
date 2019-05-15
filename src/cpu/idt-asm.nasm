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
global Idt0, Idt1, Idt2, Idt3, Idt4, Idt5, Idt6, Idt7, Idt8, Idt9, Idt10, Idt11, Idt12, Idt13, Idt14, Idt16, Idt17, Idt18, Idt19, Idt20, Idt48, Idt49, Idt50, Idt51, Idt52, Idt53, Idt54, Idt55, Idt56, Idt57, Idt58, Idt59, Idt60, Idt61, Idt62, Idt63, Idt64, Idt65, Idt66, Idt67, Idt68, Idt69, Idt70, Idt71, Idt160, Idt240, Idt241, IdtReserved, IdtCallVector, IdtCallSavedCr3, IdtCallSavedEsp, IdtCallSavedEbp, IdtCallSavedEax, IdtCallSavedEbx, IdtCallSavedEcx, IdtCallSavedEdx, IdtCallSavedEsi, IdtCallSavedEdi, IdtCallSavedEip, IdtCallSavedEflags, IdtCallCurrentPrivilegeLevel, IdtStackTop
extern IdtGates, IdtCall, MemoryPD, ApicLocalEoi

section .data
  IdtCallVector dd 0
  IdtCallSavedCr3 dd 0
  IdtCallSavedEsp dd 0
  IdtCallSavedEbp dd 0

  IdtCallSavedEax dd 0
  IdtCallSavedEbx dd 0
  IdtCallSavedEcx dd 0
  IdtCallSavedEdx dd 0
  IdtCallSavedEsi dd 0
  IdtCallSavedEdi dd 0
  IdtCallSavedEip dd 0
  IdtCallSavedEflags dd 0

  IdtCallErrorCode db 0

section .text
IdtSet:
  push ebp
  mov ebp, esp

  mov word ax, [ebp + 8]
  mov word [IdtLimit], ax
  mov dword [IdtBase], IdtGates

  lidt [Idt]

  mov esp, ebp
  pop ebp
  ret

IdtCallManage:
  mov [IdtCallSavedEbp], ebp

  cmp byte [IdtCallErrorCode], 0
  jne .Continue

  push 0    ; False error code so that the stack is added to

  .Continue:
    push eax
    mov eax, [esp + 8]
    mov [IdtCallSavedEip], eax
    mov eax, [esp + 16]
    mov [IdtCallSavedEflags], eax
    pop eax

    push eax
    mov eax, [esp + 20]
    mov [IdtCallSavedEsp], eax
    pop eax

    mov [IdtCallSavedEax], eax
    mov [IdtCallSavedEbx], ebx
    mov [IdtCallSavedEcx], ecx
    mov [IdtCallSavedEdx], edx
    mov [IdtCallSavedEsi], esi
    mov [IdtCallSavedEdi], edi

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, cr3
    mov [IdtCallSavedCr3], eax
    mov eax, [MemoryPD]
    mov cr3, eax

    ; Reset stack
    mov esp, IdtStackTop

    call IdtCall
    call ApicLocalEoi

    mov eax, [IdtCallSavedCr3]
    mov cr3, eax

    mov eax, [IdtCallSavedEax]
    mov ebx, [IdtCallSavedEbx]
    mov ecx, [IdtCallSavedEcx]
    mov edx, [IdtCallSavedEdx]
    mov esi, [IdtCallSavedEsi]
    mov edi, [IdtCallSavedEdi]

    mov ebp, [IdtCallSavedEbp]

    push eax
    mov ax, 0x20 | 0x3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    pop eax

    push 0x23 ; SS
    push dword [IdtCallSavedEsp] ; ESP
    push dword [IdtCallSavedEflags] ; EFLAGS
    push 0x1b ; CS
    push dword [IdtCallSavedEip] ; EIP

    cmp byte [IdtCallErrorCode], 0
    je .SkipErrorCode

    push 0   ; An error code will be pop'd

  .SkipErrorCode:
    iretd

Idt0:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 0
  jmp IdtCallManage

Idt1:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 1
  jmp IdtCallManage

Idt2:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 2
  jmp IdtCallManage

Idt3:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 3
  jmp IdtCallManage

Idt4:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 4
  jmp IdtCallManage

Idt5:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 5
  jmp IdtCallManage

Idt6:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 6
  jmp IdtCallManage

Idt7:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 7
  jmp IdtCallManage

Idt8:
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 8
  jmp IdtCallManage

Idt9:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 9
  jmp IdtCallManage

Idt10:
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 10
  jmp IdtCallManage

Idt11:
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 11
  jmp IdtCallManage

Idt12:
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 12
  jmp IdtCallManage

Idt13:
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 13
  jmp IdtCallManage

Idt14:
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 14
  jmp IdtCallManage

Idt16:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 16
  jmp IdtCallManage

Idt17:
  mov byte [IdtCallErrorCode], 1
  mov dword [IdtCallVector], 17
  jmp IdtCallManage

Idt18:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 18
  jmp IdtCallManage

Idt19:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 19
  jmp IdtCallManage

Idt20:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 20
  jmp IdtCallManage

Idt48:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 48
  jmp IdtCallManage

Idt49:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 49
  jmp IdtCallManage

Idt50:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 50
  jmp IdtCallManage

Idt51:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 51
  jmp IdtCallManage

Idt52:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 52
  jmp IdtCallManage

Idt53:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 53
  jmp IdtCallManage

Idt54:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 54
  jmp IdtCallManage

Idt55:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 55
  jmp IdtCallManage

Idt56:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 56
  jmp IdtCallManage

Idt57:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 57
  jmp IdtCallManage

Idt58:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 58
  jmp IdtCallManage

Idt59:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 59
  jmp IdtCallManage

Idt60:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 60
  jmp IdtCallManage

Idt61:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 61
  jmp IdtCallManage

Idt62:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 62
  jmp IdtCallManage

Idt63:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 63
  jmp IdtCallManage

Idt64:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 64
  jmp IdtCallManage

Idt65:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 65
  jmp IdtCallManage

Idt66:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 26
  jmp IdtCallManage

Idt67:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 67
  jmp IdtCallManage

Idt68:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 68
  jmp IdtCallManage

Idt69:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 69
  jmp IdtCallManage

Idt70:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 70
  jmp IdtCallManage

Idt71:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 71
  jmp IdtCallManage

Idt160:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 160
  jmp IdtCallManage

Idt240:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 240
  jmp IdtCallManage

Idt241:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 241
  jmp IdtCallManage

IdtReserved:
  mov byte [IdtCallErrorCode], 0
  mov dword [IdtCallVector], 15
  jmp IdtCallManage

section .data
Idt:
  IdtLimit dw 0         ; Limit
  IdtBase dd 0          ; Base address

section .bss
align 4
IdtStack:
  resb 8192
IdtStackTop:
