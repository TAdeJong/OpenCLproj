CXX=clang++
CXXFLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-comment
LDLIBS=-lOpenCL

.PHONY: all clean

all: helloworld

clean:
	$(RM) helloworld
