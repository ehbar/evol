# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

CPPFLAGS=-I/usr/local/include -std=c++17 -Wall -Wextra -Werror -Wno-unused-parameter -g

SRCS=Arena.cc Coord.cc EvolEngine.cc Dumper.cc Lifeform.cc Main.cc Types.cc
LDFLAGS=-L. -levol -ljson-c -lpthread

# Uncomment for SFML renderer
#LDFLAGS += -lsfml-window -lsfml-graphics -lsfml-system
#CPPFLAGS += -DEVOL_RENDERER_SFML=1
#SRCS += SFMLRenderer.cc

# Uncomment for Curses renderer
LDFLAGS += -ltinfo -lncurses
CPPFLAGS += -DEVOL_RENDERER_CURSES
SRCS += CursesRenderer.cc

CXX=g++
LIBOBJS=$(filter-out Main.o,$(SRCS:.cc=.o))
OBJS=$(LIBOBJS) Main.o

BIN=evol
LIB=libevol.a

.PHONY: default opt dbg clean distclean

default: opt

opt: CPPFLAGS += -O3
opt: $(BIN)

dbg: CPPFLAGS += -DDEBUG=1 -ggdb -fsanitize=thread -fsanitize=undefined
dbg: $(BIN)

$(BIN): $(LIB) .depend
	$(CXX) $(CPPFLAGS) Main.cc -o $(BIN) $(LDFLAGS)

$(LIB): $(LIBOBJS)
	ar -r $(LIB) $(LIBOBJS)

.depend: $(SRCS)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -MM $^ > ./.depend

include .depend

clean:
	rm -fv $(BIN) $(LIB) $(OBJS) Main.o gmon.out

distclean: clean
	rm -fv ./.depend
