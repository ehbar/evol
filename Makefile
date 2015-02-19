# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

#CPPFLAGS=-Wall -std=c++11 -g
CPPFLAGS=-Wall -std=c++11 -O3 -fno-rtti -fno-exceptions
LDFLAGS=-lncurses -ljson-c -lpthread
SRCS=Arena.cc Coord.cc CursesRenderer.cc EvolEngine.cc Dumper.cc Lifeform.cc Main.cc Random.cc Types.cc
OBJS=Arena.o Coord.o CursesRenderer.o EvolEngine.o Dumper.o Lifeform.o Main.o Random.o Types.o
CXX=clang++
BIN=evol


$(BIN): $(OBJS)
	$(CXX) $(CPPFLAGS) -o $(BIN) $(OBJS) $(LDFLAGS)

include .depend

dep: .depend

.depend: $(SRCS)
	rm -fv ./.depend
	$(CXX) $(CPPFLAGS) $(CFLAGS) -MM $^ > ./.depend

clean:
	rm -fv $(BIN) $(OBJS) gmon.out

distclean: clean
	rm -fv ./.depend
