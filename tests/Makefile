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

TEST_BIN_NAME=./a.out
include tests.mk

include ../src/objects.mk
BUILD_OBJS_LIB_TESTS=$(addsuffix -hostlib,$(addprefix ../src/,$(BUILD_OBJS_LIBRARY)))

export KERNEL_CFLAGS_TEST=-D_HYPNOTICOS_TESTS
export KERNEL_NASMFLAGS_TEST=-D_HYPNOTICOS_TESTS

INCLUDE_TESTS = ../include-tests

.PHONY: checkhost

all: checkhost prepare $(KERNEL_LIB_NAME) $(TESTS_OBJS)

checkhost:
	if [ "`uname -sm`" != "Linux x86_64" ]; then \
		echo "*** ERROR: Tests only supported on Linux x86_64 ***"; \
	  exit 1; \
	fi

prepare: checkhost
	mkdir -p $(INCLUDE_TESTS)
	$(CP) -R ../include/* $(INCLUDE_TESTS)
	cd $(INCLUDE_TESTS) && $(RM) limits.h stddef.h stdint.h stdio.h stdlib.h string.h unistd.h sys/types.h

$(KERNEL_LIB_NAME): $(BUILD_OBJS_LIB_TESTS)
	$(LOCAL_AR) rcs $@ $(BUILD_OBJS_LIB_TESTS)

%.o-hostlib: %.c
	$(LOCAL_CC) $(LOCAL_CFLAGS) $(KERNEL_CFLAGS_TEST) $(CFLAGS_KERNEL_DEFINITION) -c $< -o $@ -I../include

%.o-hostlib: %.nasm
	$(LOCAL_NASM) $(LOCAL_NASMFLAGS) $(KERNEL_NASMFLAGS_TEST) $< -o $@

%.o: %.c
	$(LOCAL_CC) $(LOCAL_CFLAGS) $< -o $(TEST_BIN_NAME) -I$(INCLUDE_TESTS) -L. -l$(KERNEL_LIB_NAME_SHORT)
	$(TEST_BIN_NAME)

install:

clean:
	$(RM) $(TEST_BIN_NAME) $(BUILD_OBJS_LIB_TESTS)
	$(RM) -R $(INCLUDE_TESTS)
