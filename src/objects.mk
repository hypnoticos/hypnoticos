#
# HypnoticOS
# Copyright (C) 2020  jk30
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# boot/multiboot.o should be at the start
BUILD_OBJS=boot/multiboot.o

BUILD_OBJS+=boot/boot.o
BUILD_OBJS+=boot/load-modules.o
BUILD_OBJS+=boot/main.o
BUILD_OBJS+=boot/multiboot-check.o
BUILD_OBJS+=boot/tss-new.o

BUILD_OBJS+=cpu/acpi.o
BUILD_OBJS+=cpu/ap-start-prepare.o
BUILD_OBJS+=cpu/ap-start.o
BUILD_OBJS+=cpu/hpet.o
BUILD_OBJS+=cpu/apic/io.o
BUILD_OBJS+=cpu/apic/local.o
BUILD_OBJS+=cpu/cpu.o
BUILD_OBJS+=cpu/cpuid.o
BUILD_OBJS+=cpu/rflags.o
BUILD_OBJS+=cpu/gdt.o
BUILD_OBJS+=cpu/idt-c.o
BUILD_OBJS+=cpu/idt-asm.o
BUILD_OBJS+=cpu/io-port.o
BUILD_OBJS+=cpu/msr.o
BUILD_OBJS+=cpu/tss.o

BUILD_OBJS+=devices/keyboard.o
BUILD_OBJS+=devices/pci.o

BUILD_OBJS+=dispatcher/dispatcher.o
BUILD_OBJS+=dispatcher/init.o
BUILD_OBJS+=dispatcher/format-elf.o

BUILD_OBJS+=function/function.o
BUILD_OBJS+=function/heap-addr.o
BUILD_OBJS+=function/heap-size.o
BUILD_OBJS+=function/new-page.o
BUILD_OBJS+=function/read.o
BUILD_OBJS+=function/sleep.o
BUILD_OBJS+=function/write.o

BUILD_OBJS+=memory/alloc.o
BUILD_OBJS+=memory/find.o
BUILD_OBJS+=memory/new-block.o
BUILD_OBJS+=memory/paging.o
BUILD_OBJS+=memory/paging-data.o
BUILD_OBJS+=memory/table.o

BUILD_OBJS+=debug.o

BUILD_OBJS+=video-memory.o
