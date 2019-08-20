CXX=g++
CFLAGS = -lm -pthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result

all: 
	$(CXX) annoy.cc -o annoy $(CFLAGS)
debug:
	$(CXX) -g annoy.cc -o annoy