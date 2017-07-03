#include<iostream>
#include<cstring>
#define SIZE 5

void optimalBST(float p[], float q[]) {
  float cost[SIZE+1][SIZE+1];
  float w[SIZE+2][SIZE+2];
  int root[SIZE+2][SIZE+2];

  for(int i=0; i<SIZE+2;i++) {
    for(int j=0; j<SIZE+2; j++) {
      cost[i][j] = 3.0;
    }
  }
  for(int i=1 ;i<=SIZE+1; i++) {
    cost[i][i-1] = q[i-1];
    w[i][i-1] = q[i-1];
  }

  
  for(int l=0; l<SIZE; l++) {
    for(int i=1;i<= SIZE-l;i++) {
    
    int j = i+l;
    float t;
    std::cout <<"i=" <<i << " j= " <<j<<"\t";
    w[i][j] = w[i][j-1] + p[j] +q[j];
      for(int r=i; r<=j; r++) {
	t = cost[i][r-1] + cost[r+1][j] + w[i][j];
	if(cost[i][j] > t) {
	  cost[i][j] = t;
	  root[i][j] = r;
	}
	
      }
 
    }
    std::cout <<std::endl;
  }
  std::cout <<"Cost Matrix " << std::endl;
  for(int l=0; l<SIZE; l++) {
    for(int i=1;i<= SIZE-l;i++) {
    
      int j = i+l;
      std::cout << cost[i][j] << " ";

    }
    std::cout << std::endl;
  }

  std::cout << "Root Matrix " <<std::endl;

  for(int l=0; l<SIZE; l++) {
    for(int i=1;i<= SIZE-l;i++) {
    
      int j = i+l;
      std::cout << root[i][j] <<" ";
    }
    std::cout << std::endl;
  }
}


int main() {
  float probabilityLeaf[]={ 0.05, 0.10, 0.05, 0.05, 0.05, 0.10 };
  float probabilityNonLeaf[] = { 0, 0.15, 0.10, 0.05, 0.10, 0.20};

  optimalBST(probabilityNonLeaf, probabilityLeaf);
}
