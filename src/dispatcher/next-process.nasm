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

global DispatcherInterrupt
extern DispatcherPrepareForNext, ApicLocalEoi, DispatcherCurrentProcessPrivilegeLevel, Tss, DispatcherCurrentPid, DispatcherNextCr3, MemoryPD

section .data
  DispatcherKernelStack_esp dd 0
  DispatcherKernelStack_ebp dd 0
  DispatcherProcessStack_esp dd 0
  DispatcherProcessStack_ebp dd 0

section .text
DispatcherInterrupt:
  cli

  push eax
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; Update CR3
  mov eax, [MemoryPD]
  mov cr3, eax
  pop eax

  ; Save current ESP & EBP (even if there has been a change in privilege level)
  mov [DispatcherProcessStack_esp], esp
  mov [DispatcherProcessStack_ebp], ebp

  ;;; 1. Check the privilege level of the last running process
  cmp byte [DispatcherCurrentProcessPrivilegeLevel], 3
  je .Step2     ; If equal, skip to step 2

  ; If PL was 0, restore ESP (not restored from TSS)
  cmp dword [DispatcherKernelStack_esp], 0
  je .Step2     ; If the saved ESP value is 0 then there was no process running before this interrupt, skip to step 2

  ; Restore ESP
  mov esp, [DispatcherKernelStack_esp]

  .Step2:
    ;;; 2. Restore EBP
    cmp dword [DispatcherKernelStack_ebp], 0
    je .Step3     ; If the saved EBP value is 0 then there was no process running before this interrupt, skip to step 3

    mov ebp, [DispatcherKernelStack_ebp]

  .Step3:
    ;;; 3. Was a process already running?
    ; If yes, save the registers
    cmp word [DispatcherCurrentPid], 0
    je .Step3a     ; Not already running, go to step 3a

    push dword [DispatcherProcessStack_ebp]  ; Structure
    push dword [DispatcherProcessStack_esp]
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    push dword 1              ; DoSave = 1

    jmp .Step4

  .Step3a:
    push dword 0      ; Push an empty structure to the stack
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0
    push dword 0      ; DoSave = 0

  .Step4:
    ;;; 4. Get next PID
    call DispatcherPrepareForNext

    ; Save ESP & EBP
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

    ; Check next privilege level
    cmp byte [DispatcherCurrentProcessPrivilegeLevel], 0
    je .Step5

    ; New privilege level is 3
    push eax
    mov ax, 0x20 | 0x03
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    pop eax

    ; Save ESP
    mov dword [Tss + 4], DispatcherKernelStack_esp

  .Step5:
    ;;; 5. Update CR3
    push eax
    mov eax, [DispatcherNextCr3]
    mov cr3, eax
    pop eax

  .Step6:
    ;;; IRETD
    iretd
