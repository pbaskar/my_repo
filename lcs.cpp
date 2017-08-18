// Longest common subsequence problem
#include<iostream>
#include<cstring>
#include<functional>

size_t lcs(char* X, char* Y)
{
  size_t lenX = strlen(X);
  size_t lenY = strlen(Y);
  size_t lcs[lenX+1][lenY+1];

  for(size_t j=0; j<=lenY; j++) {
    lcs[j][0] = 0;
    lcs[0][j]= 0;
  }

  for(size_t i=1; i<=lenX; i++) {
    for(size_t j=1; j<=lenY; j++) {

      if(X[i-1] == Y[j-1]) {
    	  lcs[i][j] = lcs[i-1][j-1] +1;

      }
      else {
    	  lcs[i][j] = std::greater<size_t>()(lcs[i-1][j], lcs[i][j-1])? lcs[i-1][j]: lcs[i][j-1];
      }

    }

  }
  return lcs[lenX][lenY];

}


int main() {
  char sequenceX[]= "ABCBDAB";
  char sequenceY[]= "BDCABA";

  std::cout << "Length of lcs " << lcs(sequenceX, sequenceY)<< std::endl;
}
