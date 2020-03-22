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

BUILD=test
BUILD_OBJS=test.o

.PHONY: install $(BUILD) clean

all: $(BUILD)

install: $(BUILD)

%.o: %.nasm
	$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD): $(BUILD_OBJS)
	$(CC) -T link.ld -o $@ $(CFLAGS) $(BUILD_OBJS) $(CRT0) $(LDFLAGS) $(LIBS)
	mkdir -p $(MEMORYDISK_ROOT)/bin
	cp $(BUILD) $(MEMORYDISK_ROOT)/bin/$(BUILD)

clean:
	$(RM) $(BUILD) $(BUILD_OBJS)
