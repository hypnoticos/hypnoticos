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
global Idt0, Idt1, Idt2, Idt3, Idt4, Idt5, Idt6, Idt7, Idt8, Idt9, Idt10, Idt11, Idt12, Idt13, Idt14, Idt16, Idt17, Idt18, Idt19, Idt20, Idt48, Idt49, Idt51, Idt52, Idt53, Idt54, Idt55, Idt56, Idt57, Idt58, Idt59, Idt60, Idt61, Idt62, Idt63, Idt64, Idt65, Idt66, Idt67, Idt68, Idt69, Idt70, Idt71, Idt160, Idt240, IdtReserved
extern IdtGates, IdtCall, IdtCallException

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

Idt0:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 0                ; #DE, Divide Error
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt1:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 1                ; #DB, Debug Exception
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt2:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 2                ; NMI Interrupt
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt3:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 3                ; #BP, Breakpoint
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt4:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 4                ; #OF, Overflow
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt5:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 5                ; #BR, BOUND Range Exceeded
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt6:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 6                ; #UD, Invalid Opcode (Undefined Opcode)
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt7:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 7                ; #NM, Device Not Available (No Math Coprocessor)
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt8:
  ; Error code on the stack
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 8                ; #DF, Double Fault
  call IdtCallException
  pop eax               ; Clear vector parameter
  iret

Idt9:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 9                ; Coprocessor Segment Overrun
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt10:
  ; Error code on the stack
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 10               ; #TS, Invalid TSS
  call IdtCallException
  pop eax               ; Clear vector parameter
  iret

Idt11:
  ; Error code on the stack
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 11               ; #NP, Segment Not Present
  call IdtCallException
  pop eax               ; Clear vector parameter
  iret

Idt12:
  ; Error code on the stack
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 12               ; #SS, Stack-Segment Fault
  call IdtCallException
  pop eax               ; Clear vector parameter
  iret

Idt13:
  ; Error code on the stack
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 13               ; #GP, General Protection
  call IdtCallException
  pop eax               ; Clear vector parameter
  iret

Idt14:
  ; Error code on the stack
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 14               ; #PF, Page Fault
  call IdtCallException
  pop eax               ; Clear vector parameter
  iret

Idt16:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 16               ; #MF, x87 FPU Floating-Point Error (Math Fault)
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt17:
  ; Error code on the stack
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 17               ; #AC, Alignment Check
  call IdtCallException
  pop eax               ; Clear vector parameter
  iret

Idt18:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 18               ; #MC, Machine Check
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt19:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 19               ; #XM, SIMD Floating-Point Exception
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt20:
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 19               ; #VE, Virtualization Exception
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt48:                  ; IRQ 0
  push 0
  push 48
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt49:                  ; IRQ 1
  push 0
  push 49
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt51:                  ; IRQ 3
  push 0
  push 51
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt52:                  ; IRQ 4
  push 0
  push 52
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt53:                  ; IRQ 5
  push 0
  push 53
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt54:                  ; IRQ 6
  push 0
  push 54
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt55:                  ; IRQ 7
  push 0
  push 55
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt56:                  ; IRQ 8
  push 0
  push 56
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt57:                  ; IRQ 9
  push 0
  push 57
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt58:                  ; IRQ 10
  push 0
  push 58
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt59:                  ; IRQ 11
  push 0
  push 59
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt60:                  ; IRQ 12
  push 0
  push 60
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt61:                  ; IRQ 13
  push 0
  push 61
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt62:                  ; IRQ 14
  push 0
  push 62
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt63:                  ; IRQ 15
  push 0
  push 63
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt64:                  ; IRQ 16
  push 0
  push 64
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt65:                  ; IRQ 17
  push 0
  push 65
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt66:                  ; IRQ 18
  push 0
  push 66
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt67:                  ; IRQ 19
  push 0
  push 67
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt68:                  ; IRQ 20
  push 0
  push 68
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt69:                  ; IRQ 21
  push 0
  push 69
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt70:                  ; IRQ 22
  push 0
  push 70
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt71:                  ; IRQ 23
  push 0
  push 71
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt160:
  push 0
  push 160
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt240:
  push 0
  push 240
  call IdtCall
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

IdtReserved:            ; 15 and 21-31
  push 0                ; No error code
  push dword [esp+8]    ; CS
  push dword [esp+8]    ; EIP
  mov eax, cr2
  push eax
  push 15               ; Reserved.
  call IdtCallException
  pop eax               ; Clear vector parameter
  pop eax               ; Clear error code parameter
  iret

Idt:
  IdtLimit dw 0         ; Limit
  IdtBase dd 0          ; Base address
