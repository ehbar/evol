# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2017 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

GTEST_PATH=/Users/eric/gtest

CPPFLAGS=-Wall -std=c++11 -g -I.. -I${GTEST_PATH}/include
LDFLAGS=-L.. -L${GTEST_PATH} -lgtest -levol
SRCS=TestMain.cc CoordTest.cc
OBJS=TestMain.o CoordTest.o
LIB=../libevol.a
CXX=g++
BIN=evol-test

.PHONY: test clean distclean

$(BIN): $(OBJS) $(LIB)
	$(CXX) $(INC) $(CPPFLAGS) -o $(BIN) $(OBJS) $(LDFLAGS)

$(LIB):
	$(MAKE) -C .. lib

include .depend

.depend: $(SRCS)
	rm -fv ./.depend
	$(CXX) $(CPPFLAGS) $(CFLAGS) -MM $^ > ./.depend

test: $(BIN)

clean:
	rm -fv $(BIN) $(OBJS)

distclean: clean
	rm -fv ./.depend
