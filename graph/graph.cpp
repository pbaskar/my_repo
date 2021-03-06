// Some graph algorithm implementation
#include "graph.hpp"
#include "heap.hpp"
#include "disjointSet.hpp"

#include<fstream>
#include<iostream>
#include<sstream>
#include<string>

#define INFINITY 100000

template <class T>
class DistanceComparator : public ItemComparator{
public:
  DistanceComparator(T* instance) {
    this->instance = instance;
  }
  bool compare(int x,int y) {
    return instance->compareDistance(x,y);
  }
private:
  T* instance;
};


void Graph::buildGraph() {
  std::ifstream file("./input.txt");
  std::string s;
  while (getline(file,s)) {
//std::cout <<s;
    std::stringstream ss(s);
    int n;
    std::list<Edge> tList;
    while(ss>>n) {
      Edge e;
      e.endVertex = n;
      ss>>e.weight;
      tList.push_back(e);
    }
    adjacencyList.push_back(tList);
  }
  distance.push_back(0);
  for(int i=1; i<adjacencyList.size(); i++) {
    distance.push_back(INFINITY);
  }
}

void Graph::print() {
  for(auto it = adjacencyList.begin(); it != adjacencyList.end(); it++) {
    for(auto innerIt = it->begin(); innerIt != it->end(); innerIt++) {
      std::cout << *innerIt << " ";
    }
    std::cout <<std::endl;
  }
}

void Graph::printDistance() {
  for(auto it = distance.begin(); it != distance.end(); it++)
    std::cout << *it << " ";
  std::cout <<std::endl;
}

bool Graph::compareDistance(int x, int y) {
  return distance[x] < distance[y];
}

void Graph::findShortestPath() {
  int nVertices = adjacencyList.size();
  Heap heap;
std::cout <<"vertices " <<nVertices <<std::endl;

  heap.setComparator(new DistanceComparator<Graph>(this));
  heap.buildHeap(nVertices);

  while(!heap.isEmpty()) {
    int closestVertex = heap.findMinimum();
    //std::cout << closestVertex << " ";
    for(auto neighbor : adjacencyList[closestVertex]) {
      if(distance[neighbor.endVertex] > distance[closestVertex] + neighbor.weight) {
        distance[neighbor.endVertex] = distance[closestVertex] +neighbor.weight;
        heap.modifyKey(neighbor.endVertex);
      }
    }
  }
}

void Graph::findConnectedComponents() {
  DisjointSet disjointSet;
  int numOfVertices = adjacencyList.size();
  disjointSet.makeSets(numOfVertices);

  for(int u=0 ;u < numOfVertices; u++) {
    for(auto edge : adjacencyList[u]) {
      if(disjointSet.findSet(u) != disjointSet.findSet(edge.endVertex)) {
	disjointSet.unionSet(u,edge.endVertex);
      }
    }
  }
  disjointSet.printSets();
}

int main() {
  Graph graph;
  graph.buildGraph();
  graph.print();

  graph.findShortestPath();
  graph.printDistance();

  graph.findConnectedComponents();
}
