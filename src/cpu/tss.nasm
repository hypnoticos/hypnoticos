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

global TssSet, TssUpdateGdt
extern Tss, TssBaseLow, TssBaseMiddle, TssBaseHigh

TssSet:
  mov ax, 0x28
  ltr ax              ; Load task register
  ret

TssUpdateGdt:
  ; Update the TSS entry in the GDT
  mov eax, Tss
  mov word [TssBaseLow], ax
  shr eax, 16
  mov byte [TssBaseMiddle], al
  mov byte [TssBaseHigh], ah
  ret
