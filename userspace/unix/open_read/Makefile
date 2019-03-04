PROG=file
OPTS=-O1

all: $(PROG)

$(PROG): $(PROG).cpp
	g++ $< -o $@ $(OPTS)

run: $(PROG)
	./$(PROG) .

clean:
	- rm $(PROG)
