# build program from 2 files and one shared header

# Compiler
CXX = g++

# Flags
CXXFLAGS = -Wall

program: main.cpp ping.cpp
	${CXX} ${CXXFLAGS} main.cpp ping.cpp -o ping

clean:
	rm -r ping
