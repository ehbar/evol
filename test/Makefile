# Part of Evol: The non-life evolution simulator.
# Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
# 
# This program is distributed under the terms of the GNU General Public
# License Version 3.  See file `COPYING' for details.

GTEST_PATH=/Users/eric/gtest

CPPFLAGS=-Wall -std=c++11 -g -I.. -I${GTEST_PATH}/include
LDFLAGS=-L${GTEST_PATH} -lgtest
SRCS=TestMain.cc CoordTest.cc
OBJS=TestMain.o CoordTest.o
CXX=clang++
BIN=evol-test


$(BIN): $(OBJS)
	$(CXX) $(INC) $(CPPFLAGS) -o $(BIN) $(OBJS) $(LDFLAGS)

include .depend

dep: .depend

.depend: $(SRCS)
	rm -fv ./.depend
	$(CXX) $(CPPFLAGS) $(CFLAGS) -MM $^ > ./.depend

clean:
	rm -fv $(BIN) $(OBJS)

distclean: clean
	rm -fv ./.depend
