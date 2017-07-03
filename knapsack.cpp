#include<iostream>
#include<functional>

int knapsack(int weights[], int len, int capacity, int values[]) {

  int maxWeight[len+1][capacity+1];

  for(int i=0; i<=capacity; i++)
    maxWeight[0][i] = 0;

  for(int i=1; i<=len; i++) {
    for(int j=0; j<=capacity; j++) {

      if(j<weights[i-1])
	maxWeight[i][j] = maxWeight[i-1][j];
      else {
	int w1= maxWeight[i-1][j];
	int w2 = maxWeight[i-1][j-weights[i-1]]+values[i-1];
	maxWeight[i][j] = std::greater<int>()(w1,w2) ? w1:w2;
      }
    }
  }
  return maxWeight[len][capacity];
}

int main() {

  int weights[] = {1,2,3};
  int len = sizeof(weights)/sizeof(int);
  int values[] = {6, 10, 12};
  int capacity = 5;

  std::cout <<knapsack(weights, len, capacity, values)<<std::endl;

}
