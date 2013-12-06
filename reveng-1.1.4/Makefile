#!/usr/bin/make
# Makefile
# Greg Cook, 28/Sep/2013

# CRC RevEng, an arbitrary-precision CRC calculator and algorithm finder
# Copyright (C) 2010, 2011, 2012, 2013  Gregory Cook
#
# This file is part of CRC RevEng.
#
# CRC RevEng is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CRC RevEng is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CRC RevEng.  If not, see <http://www.gnu.org/licenses/>.

# C compiler and flags.  Adjust to taste.
CC = gcc
CFLAGS = -O3 -Wall -ansi -fomit-frame-pointer
# Binary optimiser
STRIP = strip
SFLAGS = --strip-unneeded
# Shell commands
RM = rm
TOUCH = touch
FALSE = false
# Executable extension
EXT = .exe

# Target executable
EXE = reveng
# Target objects
TARGETS = bmpbit.o cli.o model.o poly.o reveng.o
# Header files
HEADERS = config.h reveng.h
# Pre-compiled executables and generated files
BINS = bin/i386-linux/reveng \
       bin/i386-linux/reveng$(EXT) \
       bin/raspbian/reveng \
       bin/raspbian/reveng$(EXT) \
       bin/win32/reveng \
       bin/win32/reveng$(EXT) \
       bmptst \
       bmptst$(EXT) \
       reveng \
       reveng$(EXT) \
       core

# CRC RevEng will compile without macros, but these may be useful:
# Add -DBMPMACRO to use bitmap size constant macros (edit config.h)
# Add -DNOFORCE  to disable the -F switch
# Add -DPRESETS  to compile with preset models (edit config.h)

MACROS = -DPRESETS

.PHONY: clean all

all: $(EXE)

$(EXE): $(TARGETS)
	$(MAKE) bmptst
	$(CC) $(CFLAGS) -o $@ $+
	-$(STRIP) $(SFLAGS) $@ $@$(EXT)

%.o: %.c $(HEADERS) bmptst
	$(CC) $(CFLAGS) $(MACROS) -c $<

bmptst: bmpbit.c $(HEADERS)
	$(CC) $(CFLAGS) $(MACROS) -DBMPTST -o $@ $<
	( ./$@ && $(TOUCH) $@ ) || ( $(RM) $@ $@$(EXT) && $(FALSE) )

clean:
	-$(RM) $(EXE) $(TARGETS) $(BINS)
