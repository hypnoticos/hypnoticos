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

###############################################################################
#
# BUILD_OBJS_BIN is used to exclude functions from being included in
# libhypnoticos.a.
#
# BUILD_OBJS_LIBRARY is used to place functions in libhypnoticos.a for use on
# the host system building the kernel.
#
# The default is BUILD_OBJS_BIN. Only place functions into the library
# IF IT IS NOT DANGEROUS AND IF IT IS SAFE to do so, as the tests will be
# running on the computer which builds the kernel.
#
###############################################################################

BUILD_OBJS_BIN=
BUILD_OBJS_LIBRARY=

# boot/multiboot.o should be at the start of the binary
BUILD_OBJS_BIN+=boot/multiboot.o

BUILD_OBJS_BIN+=boot/boot.o
BUILD_OBJS_BIN+=boot/load-modules.o
BUILD_OBJS_BIN+=boot/main.o
BUILD_OBJS_BIN+=boot/multiboot-check.o
BUILD_OBJS_BIN+=boot/tss-new.o

BUILD_OBJS_BIN+=cpu/acpi.o
BUILD_OBJS_BIN+=cpu/ap-start-prepare.o
BUILD_OBJS_BIN+=cpu/ap-start.o
BUILD_OBJS_BIN+=cpu/hpet.o
BUILD_OBJS_BIN+=cpu/apic/io.o
BUILD_OBJS_BIN+=cpu/apic/local.o
BUILD_OBJS_BIN+=cpu/cpu.o
BUILD_OBJS_BIN+=cpu/cpuid.o
BUILD_OBJS_BIN+=cpu/rflags.o
BUILD_OBJS_BIN+=cpu/gdt.o
BUILD_OBJS_BIN+=cpu/idt-c.o
BUILD_OBJS_BIN+=cpu/idt-asm.o
BUILD_OBJS_BIN+=cpu/io-port.o
BUILD_OBJS_BIN+=cpu/msr.o
BUILD_OBJS_BIN+=cpu/tss.o

BUILD_OBJS_BIN+=devices/storage/memory-disk/get.o
BUILD_OBJS_BIN+=devices/storage/memory-disk/read.o
BUILD_OBJS_BIN+=devices/storage/memory-disk/write.o
BUILD_OBJS_BIN+=devices/storage/get.o
BUILD_OBJS_BIN+=devices/storage/init.o
BUILD_OBJS_BIN+=devices/storage/new.o
BUILD_OBJS_BIN+=devices/storage/read.o
BUILD_OBJS_BIN+=devices/storage/write.o
BUILD_OBJS_BIN+=devices/keyboard.o
BUILD_OBJS_BIN+=devices/pci.o

BUILD_OBJS_LIBRARY+=dispatcher/dispatcher.o
BUILD_OBJS_BIN+=dispatcher/done.o
BUILD_OBJS_LIBRARY+=dispatcher/init.o
BUILD_OBJS_LIBRARY+=dispatcher/memory.o
BUILD_OBJS_LIBRARY+=dispatcher/new.o
BUILD_OBJS_BIN+=dispatcher/format-elf.o
BUILD_OBJS_LIBRARY+=dispatcher/open-indices.o

BUILD_OBJS_BIN+=fs/hypnoticfs/details-get.o
BUILD_OBJS_BIN+=fs/hypnoticfs/hypnoticfs.o
BUILD_OBJS_BIN+=fs/hypnoticfs/list.o
BUILD_OBJS_BIN+=fs/hypnoticfs/new-index.o
BUILD_OBJS_BIN+=fs/hypnoticfs/read.o
BUILD_OBJS_BIN+=fs/hypnoticfs/write.o
BUILD_OBJS_BIN+=fs/details-get.o
BUILD_OBJS_BIN+=fs/init.o
BUILD_OBJS_BIN+=fs/list.o
BUILD_OBJS_BIN+=fs/new-index.o
BUILD_OBJS_BIN+=fs/new-root.o
BUILD_OBJS_BIN+=fs/path.o
BUILD_OBJS_BIN+=fs/read.o
BUILD_OBJS_BIN+=fs/write.o

BUILD_OBJS_BIN+=function/directory-done.o
BUILD_OBJS_BIN+=function/directory-entry.o
BUILD_OBJS_BIN+=function/directory-get.o
BUILD_OBJS_BIN+=function/exit.o
BUILD_OBJS_BIN+=function/function.o
BUILD_OBJS_BIN+=function/heap-addr.o
BUILD_OBJS_BIN+=function/heap-size.o
BUILD_OBJS_BIN+=function/new-page.o
BUILD_OBJS_BIN+=function/read.o
BUILD_OBJS_BIN+=function/run.o
BUILD_OBJS_BIN+=function/sleep.o
BUILD_OBJS_BIN+=function/write.o

BUILD_OBJS_BIN+=memory/alloc.o
BUILD_OBJS_BIN+=memory/find.o
BUILD_OBJS_BIN+=memory/new-block.o
BUILD_OBJS_LIBRARY+=memory/paging.o
BUILD_OBJS_BIN+=memory/paging-data.o
BUILD_OBJS_BIN+=memory/table.o

BUILD_OBJS_BIN+=debug.o

BUILD_OBJS_LIBRARY+=lock.o

BUILD_OBJS_BIN+=video-memory.o
