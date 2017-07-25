#include<iostream>
#include<fstream>
#include<cstring>
#include "hand.hpp"
using namespace std;

void playPoker() {
  ifstream iStream("/home/priya/codeSamples/pokerGame/input.txt",ios::in);
  cout << endl <<"***********Finding Category***********"<<endl;
  while(!iStream.eof()) {
    char buf[80];
    iStream.getline(buf,80);
    if(strlen(buf) == 0) break;

    string str(buf);
    Hand* handPtr = Hand::createNewHand(str);
    if(handPtr != NULL) {
      handPtr->findCategory();
      delete handPtr;
    }
  }
  cout << endl<<"***********Find Best Permutation***********"<<endl;
  while(!iStream.eof()) {
    char buf[80];
    iStream.getline(buf,80);
    if(strlen(buf) == 0) break;

    string str(buf);
    Hand* handPtr = Hand::createNewHand(str);
    if(handPtr != NULL) {
      handPtr->findBestPermutation();
      delete handPtr;
    }
  }
  cout << endl <<"***********Finding Winner***********"<<endl;
  
  while(!iStream.eof()) {
    vector<Hand*> inputs;
    char buf[256];
    iStream.getline(buf,256);
    if(strlen(buf) == 0) break;

    char* token = strtok(buf, "/");
    while(token != NULL) {
      string str(token);
      Hand* handPtr = Hand::createNewHand(str);
      if(handPtr != NULL) {
	inputs.push_back(handPtr);
	token = strtok(NULL, "/");
      }
    }

    auto it = inputs.begin();
    Hand* winner = *(it);
    it++;
    for(;it!=inputs.end(); it++) {
      cout <<"Comparing the following two hands "<<endl;
      if(findWinner(*it, winner))
	winner = *it;
    }
    cout <<"Winner is " <<winner <<endl<<endl;

    for(auto it :inputs) {
      delete it;
    }
  } 
}

int main() {
  playPoker();
}
