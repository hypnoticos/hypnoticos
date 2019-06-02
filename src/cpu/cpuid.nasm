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

global Cpuid, CpuidTest

section .text
CpuidTest:
  ; Attempt to clear the ID bit in the EFLAGS register
  pushfq
  pop rax
  and rax, 0xFFFFFFFFFFDFFFFF
  push rax
  popfq

  ; Test if cleared
  pushfq
  pop rax
  mov rdx, rax
  push rax
  popfq

  and rdx, 0x200000
  cmp rdx, 0
  jne .Failure

  ; Attempt to set the ID bit in the EFLAGS register
  pushfq
  pop rax
  or rax, 0x200000
  push rax
  popfq

  ; Test if set
  pushfq
  pop rax
  mov rdx, rax
  push rax
  popfq

  and rdx, 0x200000
  cmp rdx, 0
  je .Failure

  jmp .Success

  .Success:
    mov rax, 1
    ret

  .Failure:
    mov rax, 0
    ret

Cpuid:
  push rbp
  mov rbp, rsp

  push rbx

  mov rax, rdi
  cpuid

  mov [_CpuidOutput_eax], eax
  mov [_CpuidOutput_ebx], ebx
  mov [_CpuidOutput_ecx], ecx
  mov [_CpuidOutput_edx], edx

  pop rbx

  mov rsp, rbp
  pop rbp
  mov rax, _CpuidOutput
  ret

section .data
align 4
_CpuidOutput:
  _CpuidOutput_eax dd 0
  _CpuidOutput_ebx dd 0
  _CpuidOutput_ecx dd 0
  _CpuidOutput_edx dd 0
