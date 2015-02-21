# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

#CPPFLAGS=-Wall -std=c++11 -g
CPPFLAGS=-Wall -std=c++11 -O3 -fno-rtti -fno-exceptions
LDFLAGS=-L. -levol -lncurses -ljson-c -lpthread
SRCS=Arena.cc Coord.cc CursesRenderer.cc EvolEngine.cc Dumper.cc Lifeform.cc Random.cc Types.cc
OBJS=Arena.o Coord.o CursesRenderer.o EvolEngine.o Dumper.o Lifeform.o Random.o Types.o
CXX=clang++
BIN=evol
LIB=libevol.a


$(BIN): $(LIB)
	$(CXX) $(CPPFLAGS) Main.cc -o $(BIN) $(LDFLAGS)

$(LIB): $(OBJS)
	ar -r $(LIB) $(OBJS)

include .depend

dep: .depend

.depend: $(SRCS)
	rm -fv ./.depend
	$(CXX) $(CPPFLAGS) $(CFLAGS) -MM $^ > ./.depend

clean:
	rm -fv $(BIN) $(LIB) $(OBJS) gmon.out

distclean: clean
	rm -fv ./.depend
