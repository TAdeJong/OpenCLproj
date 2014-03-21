CXX=clang++
CXXFLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-comment -Wno-ignored-qualifiers -std=c++0x -g
LDLIBS=-lOpenCL -lSDL -lGLEW -lGL
LINK.o=$(CXX)

SOURCES=$(wildcard *.cpp)
BINARIES=$(SOURCES:.cpp=)

.PHONY: all clean

all: $(BINARIES)

clean:
	$(RM) $(BINARIES) $(SOURCES:.cpp=.o) .deps

.deps: $(SOURCES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $(SOURCES) > $@

-include .deps
