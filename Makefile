CXX=clang++
CXXFLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-comment -std=c++0x -g
LDLIBS=-lOpenCL -lSDL -lGLEW -lGL

SOURCES=$(wildcard *.cpp)
BINARIES=$(SOURCES:.cpp=)

.PHONY: all clean

all: $(BINARIES)

clean:
	$(RM) $(BINARIES)
