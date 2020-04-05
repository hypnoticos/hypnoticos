#
# HypnoticOS
# Copyright (C) 2019  jk30
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

export VERSION=0.1
export ARCHITECTURE=x86_64
export ARCHITECTURE_UPPERCASE=$(shell echo $(ARCHITECTURE) | tr "[:lower:]" "[:upper:]")
export HYPNOTICOS=HypnoticOS-$(ARCHITECTURE)-$(VERSION)-$(shell date +"%D-%T")
export SYSROOT=$(PWD)/sysroot
export INCDIR=$(PWD)/include
export KERNELFILENAME=hypnoticos-$(ARCHITECTURE)-$(VERSION)
export KERNEL_LIB_NAME_SHORT=hypnoticos
export KERNEL_LIB_NAME=lib$(KERNEL_LIB_NAME_SHORT).a
export ISODIR=$(PWD)/iso
ISONAME=hypnoticos.iso
SUBDIRS_NOT_TESTS=libc src modules hypnoticfs-tools
SUBDIRS=$(SUBDIRS_NOT_TESTS) tests
INSTALLDIRS=$(SUBDIRS:%=install-%)
CLEANDIRS=$(SUBDIRS:%=clean-%)

export MEMORYDISK_ROOT=$(shell pwd)/memorydisk-root
MEMORYDISK=$(shell pwd)/memorydisk

export MKDIR=mkdir -p
export CP=cp
export RM=rm -f
export MAKE=make
export TARGET=$(ARCHITECTURE)-elf

export NASM ?= nasm
export NASMFLAGS ?= -f elf64
export AR ?= ar

export LOCAL_AR := $(AR)
export LOCAL_NASM := $(NASM)
export LOCAL_NASMFLAGS := $(NASMFLAGS)
export LOCAL_CC := $(CC)
export LOCAL_CFLAGS := $(CFLAGS)
export LOCAL_LDFLAGS := $(LDFLAGS)

export CC=$(TARGET)-gcc
export CFLAGS=-O2 -Wall -D_HYPNOTICOS="\"$(HYPNOTICOS)\"" -D_ARCHITECTURE_$(ARCHITECTURE_UPPERCASE) --sysroot=$(SYSROOT) -I$(INCDIR) -isystem=$(INCDIR) -mno-sse
# TODO Support larger page sizes for modules
export CFLAGS_KERNEL_DEFINITION=-D_HYPNOTICOS_KERNEL
export CFLAGS_MODULES=-O2 -Wall --sysroot=$(SYSROOT) -I$(INCDIR) -isystem=$(INCDIR) -nostdlib -mno-sse -zmax-page-size=0x1000

export AR=$(TARGET)-ar

export LD=$(TARGET)-ld
export LDFLAGS=-zmax-page-size=0x1000
export LDFLAGS_MODULES=-nostdlib -L$(PWD)/libc

export LIBS=
export LIBS_MODULES=-lgcc -lc

export NASMFLAGS=-f elf64
export NASMFLAGS_MODULES=$(NASMFLAGS)

.PHONY: install clean subdirs $(SUBDIRS) $(INSTALLDIRS) $(CLEANDIRS)

all: prepare $(SUBDIRS_NOT_TESTS)

prepare:
	which grub-mkrescue > /dev/null
	which find > /dev/null
	$(MKDIR) $(SYSROOT)/usr/include
	$(CP) -R $(INCDIR)/* $(SYSROOT)/usr/include/

subdirs: $(SUBDIRS)

$(SUBDIRS):
	+$(MAKE) -C $@

modules: libc

install: $(SUBDIRS_NOT_TESTS) $(INSTALLDIRS)

$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install

$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean

clean: $(CLEANDIRS)
	$(RM) -R $(SYSROOT) $(ISODIR) $(ISONAME) $(MEMORYDISK)

memorydisk: prepare $(SUBDIRS_NOT_TESTS)
	$(PWD)/hypnoticfs-tools/hypnoticfs-new $(MEMORYDISK) 4
	cd $(MEMORYDISK_ROOT) && find -not -path . -type d -printf "/%P\0" | xargs -n 1 -0 $(PWD)/hypnoticfs-tools/hypnoticfs-mkdir $(MEMORYDISK)
	cd $(MEMORYDISK_ROOT) && find -not -path . -not -type d -printf "/%P\0%P\0" | xargs -n 2 -0 $(PWD)/hypnoticfs-tools/hypnoticfs-add $(MEMORYDISK)

test: tests

iso: prepare $(SUBDIRS_NOT_TESTS) memorydisk
	$(MKDIR) $(ISODIR)/boot/grub $(ISODIR)/boot/hypnoticos-modules
	$(CP) $(SYSROOT)/boot/$(KERNELFILENAME) $(ISODIR)/boot/
	$(CP) $(MEMORYDISK) $(ISODIR)/boot/memorydisk
	echo "menuentry \"HypnoticOS $(VERSION)\" {" > $(ISODIR)/boot/grub/grub.cfg
	echo "multiboot /boot/$(KERNELFILENAME)" >> $(ISODIR)/boot/grub/grub.cfg
	echo "module /boot/memorydisk" >> $(ISODIR)/boot/grub/grub.cfg
	echo "}" >> $(ISODIR)/boot/grub/grub.cfg
	echo "set timeout=0" >> $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISONAME) $(ISODIR)
