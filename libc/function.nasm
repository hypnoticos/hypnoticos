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
  push ebp
  mov ebp, esp

  push ebx
  push ecx
  push edx
  push esi
  push edi

  mov eax, [ebp+8]
  mov ebx, [ebp+12]
  mov ecx, [ebp+16]
  mov edx, [ebp+20]
  mov esi, [ebp+24]
  mov edi, [ebp+28]

  int 241

  pop edi
  pop esi
  pop edx
  pop ecx
  pop ebx

  pop ebp
  ret
