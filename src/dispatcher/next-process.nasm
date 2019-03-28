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

global DispatcherInterrupt, DispatcherKernelStack
extern DispatcherPrepareForNext, ApicLocalEoi

section .data
  DispatcherKernelStack_esp dd 0
  DispatcherKernelStack_ebp dd 0
  DispatcherProcessStack_esp dd 0
  DispatcherProcessStack_ebp dd 0

section .text
DispatcherInterrupt:
  cli

  ; TODO Run processes at privelege level 3
  ; (note: the data saved to the stack isn't the same if there is a change in privelege level)

  cmp dword [DispatcherKernelStack_esp], 0
  je .Step2      ; If the stack address saved is empty then no process is running (do not save registers)

  mov [DispatcherProcessStack_esp], esp
  mov [DispatcherProcessStack_ebp], ebp

  mov esp, [DispatcherKernelStack_esp]
  mov ebp, [DispatcherKernelStack_ebp]

  push dword [DispatcherProcessStack_ebp]  ; Structure
  push dword [DispatcherProcessStack_esp]
  push edi
  push esi
  push edx
  push ecx
  push ebx
  push eax
  push dword 1              ; DoSave = 1
  jmp .Step3

  .Step2:
    push dword 0      ; Push an empty structure to the stack
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0      ; DoSave = 0

  .Step3:
    call DispatcherPrepareForNext

    mov [DispatcherKernelStack_esp], esp
    mov [DispatcherKernelStack_ebp], ebp

    ; Set up registers for the next process
    mov ebx, [eax + 4]
    mov ecx, [eax + 8]
    mov edx, [eax + 12]
    mov esi, [eax + 16]
    mov edi, [eax + 20]
    mov esp, [eax + 24]
    mov ebp, [eax + 28]
    mov eax, [eax]

    iretd
