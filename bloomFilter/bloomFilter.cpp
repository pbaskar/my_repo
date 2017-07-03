#include<iostream>
#include<iomanip>
#include "md5.hpp"
using namespace std;

class BloomFilter {
public:
  void addToDictionary(string words[], int len);
  bool isWordPresent(string word);
		      
private:
  int dict[8];
  void recordCharacter(unsigned char c);
  bool readCharacter(unsigned char c);
};

void BloomFilter::addToDictionary(string words[], int len) {
  unsigned int hash[4];
  MD5::computeHash(words[0], hash);

  for(int i=0;i<4;i++) {
    for (int j=0;j<4;j++) {
      unsigned char c = hash[i] & 0xFF;
      recordCharacter(c); 
      hash[i]=hash[i]>>8;
    }
  }
  // cout <<endl;
}

bool BloomFilter::isWordPresent(string word) {
  unsigned int hash[4];
  MD5::computeHash(word, hash);

  for(int i=0;i<4;i++) {
    for (int j=0;j<4;j++) {
      unsigned char c = hash[i] & 0xFF;
      if(!readCharacter(c)) return false; 
      hash[i]=hash[i]>>8;
    }
  }
  return true;
}
  
void BloomFilter::recordCharacter(unsigned char c) {
  int t= (unsigned int) c;
  int row = t/32;
  int col = t%32;

  dict[row] = dict[row] | 0x1 <<col;

}

bool BloomFilter::readCharacter(unsigned char c) {
  int t=(unsigned int)c;
  int row = t/32;
  int col = t%32;

  return (dict[row] & 0x1<<col) >0;
}

int main() {
  string words[] = { "Hello", "world"};

  BloomFilter bf;
  bf.addToDictionary(words, 2);

  cout << bf.isWordPresent("world") <<endl;
}
