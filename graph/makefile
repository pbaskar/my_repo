
graph : graph.hpp graph.cpp disjointSet.o heap.o
	g++ graph.hpp graph.cpp disjointSet.o heap.o -o graph -std=c++11 -g

disjointSet.o: disjointSet.hpp disjointSet.cpp
	g++ -c disjointSet.hpp disjointSet.cpp -std=c++11 -g

heap.o: heap.hpp heap.cpp
	g++ -c heap.hpp heap.cpp -std=c++11 -g
