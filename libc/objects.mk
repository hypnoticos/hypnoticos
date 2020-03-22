#
# HypnoticOS
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

LIBC_OBJS=

LIBC_OBJS+=hypnoticos/run.o

LIBC_OBJS+=stdio/fgetc.o
LIBC_OBJS+=stdio/fileno.o
LIBC_OBJS+=stdio/fprintf.o
LIBC_OBJS+=stdio/fputc.o
LIBC_OBJS+=stdio/fputs.o
LIBC_OBJS+=stdio/getc.o
LIBC_OBJS+=stdio/getchar.o
LIBC_OBJS+=stdio/printf.o
LIBC_OBJS+=stdio/putchar.o
LIBC_OBJS+=stdio/puts.o
LIBC_OBJS+=stdio/snprintf.o
LIBC_OBJS+=stdio/stdstreams.o
LIBC_OBJS+=stdio/vfprintf.o
LIBC_OBJS+=stdio/vsnprintf.o

LIBC_OBJS+=stdlib/calloc.o
LIBC_OBJS+=stdlib/free.o
LIBC_OBJS+=stdlib/malloc.o
LIBC_OBJS+=stdlib/realloc.o

LIBC_OBJS+=string/memcmp.o
LIBC_OBJS+=string/memcpy.o
LIBC_OBJS+=string/memset.o
LIBC_OBJS+=string/strcat.o
LIBC_OBJS+=string/strcmp.o
LIBC_OBJS+=string/strcpy.o
LIBC_OBJS+=string/strlen.o

LIBC_OBJS+=unistd/read.o
LIBC_OBJS+=unistd/sleep.o
LIBC_OBJS+=unistd/write.o
