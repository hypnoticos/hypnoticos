#!/bin/bash
#
# HypnoticFS Tools
# Copyright (C) 2019, 2020  jk30
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

DESTDIR ?= /usr/local

MKDIR ?= mkdir -p
MAKE ?= make
CP ?= cp
RM ?= rm
LOCAL_CC ?= gcc
LOCAL_CFLAGS ?= -O2 -Wall
LOCAL_LDFLAGS ?=

HYPNOTICFS_ADD=hypnoticfs-add
HYPNOTICFS_ADD_SRC=hypnoticfs-add.o common.o

HYPNOTICFS_MKDIR=hypnoticfs-mkdir
HYPNOTICFS_MKDIR_SRC=hypnoticfs-mkdir.o common.o

HYPNOTICFS_NEW=hypnoticfs-new
HYPNOTICFS_NEW_SRC=hypnoticfs-new.o

all: $(HYPNOTICFS_ADD) $(HYPNOTICFS_MKDIR) $(HYPNOTICFS_NEW)

%.o: %.c
	$(LOCAL_CC) $(LOCAL_CFLAGS) -c $< -o $@

$(HYPNOTICFS_ADD): $(HYPNOTICFS_ADD_SRC)
	$(LOCAL_CC) $(LOCAL_LDFLAGS) -o $(HYPNOTICFS_ADD) $(HYPNOTICFS_ADD_SRC)

$(HYPNOTICFS_MKDIR): $(HYPNOTICFS_MKDIR_SRC)
	$(LOCAL_CC) $(LOCAL_LDFLAGS) -o $(HYPNOTICFS_MKDIR) $(HYPNOTICFS_MKDIR_SRC)

$(HYPNOTICFS_NEW): $(HYPNOTICFS_NEW_SRC)
	$(LOCAL_CC) $(LOCAL_LDFLAGS) -o $(HYPNOTICFS_NEW) $(HYPNOTICFS_NEW_SRC)

clean:
	$(RM) $(HYPNOTICFS_ADD) $(HYPNOTICFS_MKDIR) $(HYPNOTICFS_NEW)
	$(RM) $(HYPNOTICFS_ADD_SRC) $(HYPNOTICFS_MKDIR_SRC) $(HYPNOTICFS_NEW_SRC)
