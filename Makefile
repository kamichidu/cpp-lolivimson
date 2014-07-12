CXX=clang++
CXXFLAGS=-std=c++11 -Wall -O2
TARGET=bin/run-test
SRC=t/test.cpp
INCDIR=-I./src/

test:
	${CXX} ${CXXFLAGS} ${INCDIR} -o ${TARGET} ${SRC}
