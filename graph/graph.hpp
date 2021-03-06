#include<list>
#include<vector>
#include<iostream>

struct Edge {
  int endVertex;
  int weight;

  friend std::ostream& operator<<(std::ostream& out, Edge e) {
    out << e.endVertex <<"/"<<e.weight<<" ";
    return out;
  }
};
class Graph {
public:
  void buildGraph();
  void findShortestPath();
  void findConnectedComponents();
  void print();
  void printDistance();

  bool compareDistance(int x, int y);

private:
  std::vector<std::list<Edge> > adjacencyList;
  std::vector<int> distance;
};
