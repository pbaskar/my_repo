// Rod cutting problem
#include<iostream>
#include<cstring>
#define SIZE 10

void cutRodBottomUp(int n, int prices[], int costs[]) {
  for(int i=1; i<n; i++) {
    for(int j=1; j<i; j++) {
      int cost = prices[j] + costs[i-j-1];
      if(costs[i] < cost)
	costs[i] = cost;
    }
  }
}


int main() {
  int prices[]= {1,5,8,9,10,17,17,20,24,30};
  int costs[SIZE];

  int n = 8;   // for size n rod
  std::memcpy(costs, prices, SIZE<<2);
  cutRodBottomUp (n, prices, costs);
  for(auto cost:costs)
    std::cout <<cost <<" ";
  std::cout<<std::endl;
}
