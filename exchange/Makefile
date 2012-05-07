#!/bin/sh

inc=./inc
src=./src
test=./test
bin=`pwd`
exe=exchange
unitTests=unitTests

CC=/usr/bin/g++
INCL=-I/usr/include -I/usr/include/c++/4.2.1 -I$(inc)
LIB=/usr/lib
CFLAGS=
LDFLAGS=

sources = $(wildcard $(src)/*.cpp)
objects	= $(sources:.cpp=.o)

testObjs= $(src)/OrderManager.o $(test)/testCases.o

default: all

compile:
	$(CC) -c $(CFLAGS) $(src)/OrderManager.cpp $(INCL) -o $(src)/OrderManager.o
	$(CC) -c $(CFLAGS) $(src)/main.cpp $(INCL) -o $(src)/main.o
	@echo "Compiled"
link:
	$(CC) -o $(exe) $(objects) $(CFLAGS) $(LDFLAGS)
	@echo "Linked"

all: clean compile link test
	@echo "Built All!"

test: compile
	$(CC) -c $(CFLAGS) $(test)/testCases.cpp $(INCL) -o $(test)/testCases.o
	$(CC) -o $(unitTests) $(testObjs) $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(objects)
	rm -f $(exe)
	rm -f $(testObjs)
	rm -f $(unitTests)
