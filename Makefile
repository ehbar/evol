# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

#CPPFLAGS=-Wall -std=c++11 -g
CPPFLAGS=-Wall -std=c++11 -O3 -fno-rtti -fno-exceptions
LDFLAGS=-L. -levol -lncurses -ljson-c -lpthread
CXX=clang++
SRCS=Arena.cc Coord.cc CursesRenderer.cc EvolEngine.cc Dumper.cc Lifeform.cc Main.cc Random.cc Types.cc
LIBOBJS=Arena.o Coord.o CursesRenderer.o EvolEngine.o Dumper.o Lifeform.o Random.o Types.o
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
