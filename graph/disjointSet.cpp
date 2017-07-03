#include "disjointSet.hpp"
#include<algorithm>

Set* DisjointSet::makeSet(int value) {
  Node* newNode = new Node();
  newNode->value = value;

  Set* newSet = new Set();
  newSet->head = newNode;
  newSet->tail = newNode;
  newSet->size = 1;

  newNode->set = newSet;
  sets.push_back(newSet);
  nodes.push_back(newNode);
  return newSet;
}

Set* DisjointSet::unionSet(int value1, int value2) {
  Set* s1 = findSet(value1);
  Set* s2 = findSet(value2);

  if(s1->size < s2->size) {
      Set* temp = s1;
      s1 = s2;
      s2 = temp;
  }
    
  s1->size += s2->size;

  s1->tail->next = s2->head;
  s1->tail = s2->tail;

  Node* current = s2->head;
  while(current != nullptr) {
    current->set= s1;
    current = current->next;
  }

  auto deleteSet = find(sets.begin(), sets.end(), s2);
  sets.erase(deleteSet);
  delete s2;
  return s1;
}

Set* DisjointSet::findSet(int value) {
  return nodes[value]->set;
  
}

Set* DisjointSet::makeSets(int size) {
  for(int i=0;i<size; i++)
    makeSet(i);
}

void DisjointSet::printSets() {
  for(auto set : sets )
    std::cout<<*set;
}
