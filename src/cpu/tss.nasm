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

global Tss
extern IdtStackTop

section .data
align 4096
Tss:
  _Tss_Start equ $
  dd 0                  ; Reserved

  dq IdtStackTop        ; RSP0
  dq 0                  ; RSP1
  dq 0                  ; RSP2

  dq 0                  ; Reserved

  dq 0                  ; IST1
  dq 0                  ; IST2
  dq 0                  ; IST3
  dq 0                  ; IST4
  dq 0                  ; IST5
  dq 0                  ; IST6
  dq 0                  ; IST7

  dq 0                  ; Reserved
  dw 0                  ; Reserved

  dw _Tss_IoMap - _Tss_Start  ; I/O Map Base
  times 32 db 0         ; Int. redirection
  _Tss_IoMap equ $
  times 8192 db 0xFF    ; I/O Map
  db 0xFF               ; Final byte of I/O Map
