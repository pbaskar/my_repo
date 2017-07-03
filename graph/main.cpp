#include<iostream>

#include "heap.hpp"

int distance[] = {2,3,1,4,5};

bool compare(int x,int y) {
  return distance[x] < distance[y];
}

int main() {

  Heap heap;
  heap.setComparator(&compare);
  heap.buildHeap(5);
  heap.print();

  distance[4] = 0;
  heap.print();

  heap.modifyKey(4);
  heap.print();
}
