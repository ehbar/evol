# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

BASECPP=-I/usr/local/include -Wall -std=c++17 -fno-rtti -fno-exceptions
# Debug
#CPPFLAGS=$(BASECPP) -DDEBUG=1 -g -fno-rtti -fno-exceptions
# Opt
CPPFLAGS=$(BASECPP) -O3 -fno-rtti -fno-exceptions

SRCS=Arena.cc Coord.cc EvolEngine.cc Dumper.cc Lifeform.cc Main.cc Random.cc Types.cc
LDFLAGS=-L. -levol -ljson-c -lpthread
# SFML
#LDFLAGS += -lsfml-window -lsfml-graphics -lsfml-system
#CPPFLAGS += -DEVOL_RENDERER_SFML=1
#SRCS += SFMLRenderer.cc
# Curses
LDFLAGS += -ltinfo -lncurses
CPPFLAGS += -DEVOL_RENDERER_CURSES
SRCS += CursesRenderer.cc

CXX=g++
LIBOBJS=$(filter-out Main.o,$(SRCS:.cc=.o))
OBJS=$(LIBOBJS) Main.o

BIN=evol
LIB=libevol.a

.PHONY: bin lib clean distclean test

$(BIN): $(LIB) .depend
	$(CXX) $(CPPFLAGS) Main.cc -o $(BIN) $(LDFLAGS)

$(LIB): $(LIBOBJS)
	ar -r $(LIB) $(LIBOBJS)

bin: $(BIN)

lib: $(LIB)

.depend: $(SRCS)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -MM $^ > ./.depend

include .depend

test/evol-test: $(LIB)
	$(MAKE) -C test

test: test/evol-test
	test/evol-test

clean:
	rm -fv $(BIN) $(LIB) $(OBJS) Main.o gmon.out

distclean: clean
	rm -fv ./.depend
	$(MAKE) -C test distclean
