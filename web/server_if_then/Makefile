CFLAGS += -I../include

all: server

server: server.cpp
	$(CXX) $(CFLAGS) $< -o $@ -lpthread

clean:
	- rm server
