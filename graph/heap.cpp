#include<iostream>
#include<algorithm>
#include "heap.hpp"

void swap(int& x, int& y) {
  int temp;
  temp = x;
  x = y;
  y = temp;
}

int Heap::getIndexForKey(int key) {
  return find(input.begin(), input.end(), key)-input.begin();
}

void Heap::modifyKey(int key) {
  int index = getIndexForKey(key);

  while(index >0 && comparator->compare(input[index],input[parent(index)])){
    swap(input[index], input[parent(index)]);
    index = parent(index);
  } 
}

void Heap::setComparator(ItemComparator* comparator) {
  this->comparator = comparator;
}

bool Heap::isEmpty() {
  return input.empty();
}

int Heap::findMinimum() {
  int minimum = input[0];
  swap(input[0], input.back());
  input.pop_back();
  heapify(0);
  return minimum;
}

void Heap::print() {
  for(int i=0 ;i <5;i++)
    std::cout <<input[i] <<" ";
  std::cout<<std::endl;
}

void Heap::heapify(int index) {
  int l = left(index);
  int r = right(index);
  int min = index;

  int length = input.size();
  if (l<length && comparator->compare(input[l],input[min])) {
    min =l;
  }
  if (r<length && comparator->compare(input[r],input[min])) {
    min =r;
  }
  if (min != index) {
    swap(input[min], input[index]);
    heapify(min);
  }
}

void Heap::buildHeap(int length) {
  for(int i=0; i<length; i++)
    input.push_back(i);
  for(int i=length/2; i>=0; i--) {
    heapify(i);
  }
}

int Heap::left(int n) {
  return 2*n+1;
}

int Heap::right(int n) {
  return 2*n+2;
}

int Heap::parent(int n) {
  return (n-1)/2;
}
