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

SUBDIRS=test
INSTALLDIRS=$(SUBDIRS:%=install-%)
CLEANDIRS=$(SUBDIRS:%=clean-%)

export CFLAGS=$(CFLAGS_MODULES)
export NASMFLAGS=$(NASMFLAGS_MODULES)
export LDFLAGS=$(LDFLAGS_MODULES)
export LIBS=$(LIBS_MODULES)
export CRT0=$(shell pwd)/crt0.o

.PHONY: install clean subdirs $(SUBDIRS) $(INSTALLDIRS) $(CLEANDIRS)

all: $(CRT0) subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

%.o: %.nasm
	$(NASM) $(NASMFLAGS) $< -o $@

install: subdirs $(INSTALLDIRS)

$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install

$(CLEANDIRS):
	rm -f $(CRT0)
	$(MAKE) -C $(@:clean-%=%) clean

clean: $(CLEANDIRS)
