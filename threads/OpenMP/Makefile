CFLAGS=-fopenmp

all: run

run: main
	./main

main.o: main.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

main: main.o
	$(CXX) $(CFLAGS) $< -o $@ 

clean:
	- rm main main.o
