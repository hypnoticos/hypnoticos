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
global Idt0, Idt1, Idt2, Idt3, Idt4, Idt5, Idt6, Idt7, Idt8, Idt9, Idt10, Idt11, Idt12, Idt13, Idt14, Idt16, Idt17, Idt18, Idt19, Idt20, IdtReserved
extern IdtGates, IdtCall

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
  push 0                ; #DE, Divide Error
  call IdtCall

Idt1:
  push 0                ; No error code
  push 1                ; #DB, Debug Exception
  call IdtCall

Idt2:
  push 0                ; No error code
  push 2                ; NMI Interrupt
  call IdtCall

Idt3:
  push 0                ; No error code
  push 3                ; #BP, Breakpoint
  call IdtCall

Idt4:
  push 0                ; No error code
  push 4                ; #OF, Overflow
  call IdtCall

Idt5:
  push 0                ; No error code
  push 5                ; #BR, BOUND Range Exceeded
  call IdtCall

Idt6:
  push 0                ; No error code
  push 6                ; #UD, Invalid Opcode (Undefined Opcode)
  call IdtCall

Idt7:
  push 0                ; No error code
  push 7                ; #NM, Device Not Available (No Math Coprocessor)
  call IdtCall

Idt8:
  ; Error code on the stack
  push 8                ; #DF, Double Fault
  call IdtCall

Idt9:
  push 0                ; No error code
  push 9                ; Coprocessor Segment Overrun
  call IdtCall

Idt10:
  ; Error code on the stack
  push 10               ; #TS, Invalid TSS
  call IdtCall

Idt11:
  ; Error code on the stack
  push 11               ; #NP, Segment Not Present
  call IdtCall

Idt12:
  ; Error code on the stack
  push 12               ; #SS, Stack-Segment Fault
  call IdtCall

Idt13:
  ; Error code on the stack
  push 13               ; #GP, General Protection
  call IdtCall

Idt14:
  ; Error code on the stack
  push 14               ; #PF, Page Fault
  call IdtCall

Idt16:
  push 0                ; No error code
  push 16               ; #MF, x87 FPU Floating-Point Error (Math Fault)
  call IdtCall

Idt17:
  ; Error code on the stack
  push 17               ; #AC, Alignment Check
  call IdtCall

Idt18:
  push 0                ; No error code
  push 18               ; #MC, Machine Check
  call IdtCall

Idt19:
  push 0                ; No error code
  push 19               ; #XM, SIMD Floating-Point Exception
  call IdtCall

Idt20:
  push 0                ; No error code
  push 19               ; #VE, Virtualization Exception
  call IdtCall

IdtReserved:            ; 15 and 21-31
  push 0                ; No error code
  push 15               ; Reserved.
  call IdtCall

Idt:
  IdtLimit dw 0         ; Limit
  IdtBase dd 0          ; Base address
