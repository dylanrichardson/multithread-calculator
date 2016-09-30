all: graph

graph: graph.o scheduler.o node.o
	g++ -o graph graph.o scheduler.o node.o -lpthread -Wall

graph.o: graph.cpp scheduler.o node.o
	g++ -c graph.cpp -Wall

scheduler.o: scheduler.cpp scheduler.hpp node.o
	g++ -c scheduler.cpp -Wall

node.o: node.cpp node.hpp
	g++ -c node.cpp -Wall

clean:
	rm -f graph graph.o scheduler.o node.o
