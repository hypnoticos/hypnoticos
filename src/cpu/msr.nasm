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

MsrWrite:
  push ebp
  mov ebp, esp

  mov ecx, [ebp + 8]
  mov edx, [ebp + 12]
  mov eax, [ebp + 16]
  wrmsr

  mov esp, ebp
  pop ebp
  mov eax, 0
  ret

MsrRead:
  push ebp
  mov ebp, esp

  mov ecx, [ebp + 8]
  rdmsr

  mov [_MsrReadOutput_edx], edx
  mov [_MsrReadOutput_eax], eax

  mov esp, ebp
  pop ebp
  mov eax, _MsrReadOutput
  ret

_MsrReadOutput:
  _MsrReadOutput_edx dd 0
  _MsrReadOutput_eax dd 0
