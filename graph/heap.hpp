#include<vector>

class ItemComparator {
public:
  virtual bool compare(int, int)=0;
};
class Heap {

public:
  void modifyKey(int key);
  void buildHeap(int length);
  void setComparator(ItemComparator* comparator);
  void print();
  bool isEmpty();
  int findMinimum();

  ~Heap() {
    delete comparator;
  }

private:
  std::vector<int> input;
  ItemComparator* comparator;
  
  void heapify(int index);
  int left(int n);
  int right(int n);
  int parent(int n);
  int getIndexForKey(int key);
};
