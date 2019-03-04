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

CpuidTest:
  ; Attempt to clear the ID bit in the EFLAGS register
  pushfd
  pop eax
  and eax, 0xFFDFFFFF
  push eax
  popfd

  ; Test if cleared
  pushfd
  pop eax
  mov edx, eax
  push eax
  popfd

  and edx, 0x200000
  cmp edx, 0
  jne .Failure

  ; Attempt to set the ID bit in the EFLAGS register
  pushfd
  pop eax
  or eax, 0x200000
  push eax
  popfd

  ; Test if set
  pushfd
  pop eax
  mov edx, eax
  push eax
  popfd

  and edx, 0x200000
  cmp edx, 0
  je .Failure

  jmp .Success

  .Success:
    mov eax, 1
    ret

  .Failure:
    mov eax, 0
    ret

Cpuid:
  push ebp
  mov ebp, esp

  mov eax, [ebp + 8]
  cpuid

  mov [_CpuidOutput_eax], eax
  mov [_CpuidOutput_ebx], ebx
  mov [_CpuidOutput_ecx], ecx
  mov [_CpuidOutput_edx], edx

  mov esp, ebp
  pop ebp
  mov eax, _CpuidOutput
  ret

_CpuidOutput:
  _CpuidOutput_eax dd 0
  _CpuidOutput_ebx dd 0
  _CpuidOutput_ecx dd 0
  _CpuidOutput_edx dd 0
