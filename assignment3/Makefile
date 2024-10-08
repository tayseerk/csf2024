CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

.PHONY: all clean

all: csim

csim: main.o
    $(CXX) $(CXXFLAGS) -o csim main.o

main.o: main.cpp
    $(CXX) $(CXXFLAGS) -c main.cpp

clean:
    rm -f csim main.o
