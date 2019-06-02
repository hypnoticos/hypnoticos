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

global KernelFunctionInterface

section .text
KernelFunctionInterface:
  push rbp
  mov rbp, rsp

  push rbx
  push rcx
  push rdx
  push rsi
  push rdi

  mov rax, rdi
  mov rbx, rsi
  mov rcx, rdx
  mov rdx, rcx
  mov rsi, r8
  mov rdi, r9

  int 241

  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx

  pop rbp
  ret
