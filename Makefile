CPP = g++
SRC = drawing.cpp utils.cpp microseconds.c
FLAGS = -pthread -Ofast -static -std=c++0x
LIBS = -lrt -lm

all: drawing

drawing: drawing.cpp utils.cpp utils.h microseconds.c
	$(CPP) $(FLAGS) -o drawing $(SRC) $(LIBS)

clean:
	rm -f drawing

.PHONY: all clean
