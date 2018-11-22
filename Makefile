# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

#CPPFLAGS=-I/usr/local/include -Wall -DDEBUG=1 -std=c++14 -g -O1 -fno-rtti -fno-exceptions
CPPFLAGS=-I/usr/local/include -Wall -std=c++14 -O3 -fno-rtti -fno-exceptions
LDFLAGS=-L. -levol -ltinfo -ljson-c -lpthread -lsfml-window -lsfml-graphics -lsfml-system #-lncurses
CXX=g++
SRCS=Arena.cc Coord.cc EvolEngine.cc Dumper.cc Lifeform.cc Main.cc Random.cc Types.cc SFMLRenderer.cc # CursesRenderer.cc
LIBOBJS=Arena.o Coord.o EvolEngine.o Dumper.o Lifeform.o Random.o Types.o SFMLRenderer.o # CursesRenderer.o
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
