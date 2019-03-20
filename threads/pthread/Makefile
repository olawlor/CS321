all: run

run: main
	./main

main: main.o
	$(CXX) $< -o $@ -lpthread

clean:
	- rm main main.o
