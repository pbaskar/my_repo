#include<iostream>

class MD5 {
public:
  static unsigned int leftRotate(unsigned int s, int c);
  static void computeHash(std::string message, unsigned int hash[4]);
};
