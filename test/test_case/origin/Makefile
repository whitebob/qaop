CC = g++
CFLAGS = --std=c++11 -O2

all: output
	
config.hpp: queue.hpp

output: client.cpp config.hpp
	$(CC) $(CFLAGS) -o output client.cpp 
clean:
	rm -f output 
