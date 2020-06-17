# build program from 2 files and one shared header

# Compiler
CXX = g++

# Flags
CXXFLAGS = -Wall

program: main.cc ping.cc
	${CXX} ${CXXFLAGS} main.cc ping.cc -o ping

clean:
	rm -r ping

debug:
	${CXX} ${CXXFLAGS} -g main.cc ping.cc -o pingDebug

clean-debug:
	rm -r pingDebug
