CPP = g++
SRC = drawing.cpp utils.cpp microseconds.c
FLAGS = -pthread -O3 -static -std=c++0x

all: drawing

drawing: drawing.cpp utils.cpp utils.h microseconds.c
	$(CPP) $(FLAGS) -o drawing $(SRC)

clean:
	rm -f drawing

.PHONY: all clean
