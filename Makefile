CXX=clang++
CXXFLAGS=-Wall -Wextra -pedantic

.PHONY: all clean

all: helloworld

clean:
	$(RM) helloworld
