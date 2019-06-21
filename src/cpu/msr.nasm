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

global MsrRead, MsrWrite

; TODO Check if MSR is supported

section .text
MsrWrite:
  push rbp
  mov rbp, rsp

  mov ecx, edi
  mov edx, esi
  mov eax, edx
  wrmsr

  mov rsp, rbp
  pop rbp
  mov rax, 0
  ret

MsrRead:
  push rbp
  mov rbp, rsp

  mov ecx, edi
  rdmsr

  mov [_MsrReadOutput_edx], edx
  mov [_MsrReadOutput_eax], eax

  mov rsp, rbp
  pop rbp
  mov rax, _MsrReadOutput
  ret

section .data
_MsrReadOutput:
  _MsrReadOutput_edx dd 0
  _MsrReadOutput_eax dd 0
