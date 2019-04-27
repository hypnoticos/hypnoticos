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

include objects.mk

BUILD=libc.a
BUILD_OBJS=$(LIBC_OBJS)
BUILD_OBJS+=function.o

CFLAGS+=-D_HYPNOTICOS_LIBC

.PHONY: install $(BUILD) clean

all: $(BUILD)

install: $(BUILD)

%.o: %.nasm
	$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD): $(BUILD_OBJS)
	$(AR) rcs $@ $(BUILD_OBJS)
	mkdir -p $(SYSROOT)/lib
	cp $(BUILD) $(SYSROOT)/lib

clean:
	$(RM) $(BUILD) $(BUILD_OBJS)
