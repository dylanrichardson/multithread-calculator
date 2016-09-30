all: nblock

nblock: graph.o scheduler.o node.o nblock.o
	g++ -o nblock graph.o scheduler.o node.o nblock.o -lpthread -Wall

graph.o: graph.cpp scheduler.o node.o
	g++ -c graph.cpp -Wall

scheduler.o: scheduler.cpp scheduler.hpp node.o
	g++ -c scheduler.cpp -Wall

node.o: node.cpp node.hpp
	g++ -c node.cpp -Wall

nblock.o: nblock.cpp nblock.hpp
	g++ -c nblock.cpp -Wall

clean:
	rm -f nblock graph.o scheduler.o node.o nblock.o
