#include<vector>
#include<iostream>
struct Set;
struct Node {
  int value;
  Set* set;
  Node* next;
  Node() {
    set = nullptr;
    next = nullptr;
  }
};
struct Set {
  Node* head;
  Node* tail;
  int size;
  Set() {
    head = nullptr;
    tail = nullptr;
  }
  ~Set() {
  }
  friend std::ostream& operator<<(std::ostream& out, Set set) {
    Node* current = set.head;
    while(current != nullptr) {
      out << current->value <<" ";
      current = current->next;
    }
    out <<std::endl;
  }
};
class DisjointSet {
public:
  Set* makeSets(int size);
  Set* unionSet(int value1, int value2);
  Set* findSet(int value);
  void printSets();
  ~DisjointSet() {
    for(auto node : nodes)
      delete node;
    nodes.clear();
  }
private:
  std::vector<Node*> nodes;
  std::vector<Set*> sets;
  Set* makeSet(int value);
};
