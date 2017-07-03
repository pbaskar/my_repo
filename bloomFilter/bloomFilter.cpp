#include<iostream>
#include<iomanip>
#include<fstream>
#include<algorithm>
#include<iterator>
#include "md5.hpp"

#define MAXSIZE 100000
using namespace std;

class BloomFilter {
public:
  void addToDictionary(string words[], int len);
  bool isWordPresent(string word);
		      
private:
  unsigned int dict[8];
  void recordCharacter(unsigned char c);
  bool readCharacter(unsigned char c);
};

void BloomFilter::addToDictionary(string words[], int len) {
  unsigned int hash[4];
  for(int i=0; i<1; i++) {
    MD5::computeHash(words[i], hash);
    for(int i=0;i<4;i++) {
      //for (int j=0;j<4;j++) {
	unsigned char c = hash[i] & 0xFF;
	recordCharacter(c); 
	//hash[i]=hash[i]>>8;
	//}
    }
  }
}

bool BloomFilter::isWordPresent(string word) {
  unsigned int hash[4];
  MD5::computeHash(word, hash);
  for(int i=0;i<4;i++) {
    //for (int j=0;j<4;j++) {
      unsigned char c = hash[i] & 0xFF;
      if(!readCharacter(c)) return false; 
      //hash[i]=hash[i]>>8;
      //}
  }
  return true;
}
  
void BloomFilter::recordCharacter(unsigned char c) {
  int t= (unsigned int) c;
  int row = t/32;
  int col = t%32;

  dict[row] = dict[row] | ((unsigned int)(0x1) <<col);

}

bool BloomFilter::readCharacter(unsigned char c) {
  int t=(unsigned int)c;
  int row = t/32;
  int col = t%32;
  return ((dict[row] & ((unsigned int)(0x1)<<col)) >0);
}

int main() {
  string words[MAXSIZE];
  int index=0;
  
  ifstream inFile("/usr/share/dict/words");
  for(auto it=istream_iterator<string>(inFile); it != istream_iterator<string>(); it++) {
    words[index] = *it;
    index++;
  }

  BloomFilter bf;
  bf.addToDictionary(words, index);

  cout << bf.isWordPresent("A") <<endl;
}
