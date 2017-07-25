#include<regex>

#include "hand.hpp"

bool Hand::compareCards(const card* c1, const card* c2) {
  return getNumRank(c1->rank) > getNumRank(c2->rank);
}

/*
  Parse the input string into Hand object and preprocess hand
*/
Hand* Hand::createNewHand(string input) {
  int r; char s;
  regex e("[2-9JKQAT][SHCDo]");
  smatch matches;

  Hand* newHand = new Hand();
  newHand->originalHand = input;
  while(regex_search(input,matches, e)) {
    for(auto m:matches) {
      card* newCard = new card();
      if(m.str() == "Jo") {
	newCard->rank = 'Y';
      }
      else {
	newCard->rank = m.str().at(0);
	newCard->suit = m.str().at(1);
      }
      
      newHand->addCard(newCard);
    }
    input = matches.suffix().str();
  }
  if(newHand->pHand.size() <5) {
    cout << newHand << " : Invalid input" <<endl;
    delete newHand;
    return NULL;
  }
  newHand->preprocessHand();
  return newHand;
}

Hand::~Hand() {
  for(auto it:pHand) {
    delete it;
  }
}

void Hand::addCard(card* c) {
  pHand.push_back(c);
}

void Hand::addJoker(card* c) {
  jokers.push_back(c);
}

void Hand::extractJokers() {
  for(auto it=pHand.begin(); it!= pHand.end();) {
    if((*it)->isJoker()) {
      addJoker(*it);
      it=pHand.erase(it);
    }
    else {
      it++;
    }
  }
}

/*
  1.Sort the list of hands
  2.Move the cards of same rank that have maximum count to topKind
  3.Add remaining cards to kickers
*/
void Hand::preprocessHand() {
  extractJokers();
  pHand.sort(compareCards);
  list<card*> tempHand(pHand);
  int topKindCountIndex =0;
  while(!tempHand.empty()) {
      
    int maxCount =1;
    int count=1;
    list<card*>::iterator maxIterator=tempHand.begin();
    list<card*>::iterator  beginIterator=tempHand.begin();
    auto it = tempHand.begin();
    char previous = (*it)->rank;

    it++;
    for(;it != tempHand.end(); it++) {
      if((*it)->rank == previous) {
	count++;
	if (count > maxCount) {
	  maxCount = count;
	  maxIterator = beginIterator;
	}
      }
      else {
	count=1;
	beginIterator = it;
      }
      previous = (*it)->rank;
    }
    list<card*>::iterator endIterator = maxIterator;
    std::advance(endIterator,maxCount);
      
    if(topKindCountIndex < 2) {
      topKind[topKindCountIndex].insert(topKind[topKindCountIndex].begin(), maxIterator, endIterator);
      topKindCountIndex++;
    }
    else {
      kickers.insert(kickers.begin(), maxIterator, endIterator);
    }      
    tempHand.erase(maxIterator, endIterator);
  }
  sort(kickers.begin(), kickers.end(), compareCards);
}

ostream& operator<<(ostream& out , const Hand* h) {
  out << h->originalHand;
}

/*
  Apply the rules for different hand categories and deduce hand category
  isGreedy - To cover permutation case to find the best possible hand
*/
categories Hand::findCategory(bool isGreedy) const{
  categories category;
  cout << this << " : " ;
  if (isGreedy) {
    list<card*> tempHand(pHand);

    for(char suit : "CSDH") {
      for(auto it = tempHand.begin();it != tempHand.end();) {
	if(isStraight(tempHand, suit)) {	  
	  if(getNumRank(tempHand.front()->rank) == 14) {
	    cout <<"Royal Flush " << endl;
	    return ROYAL_FLUSH;
	  }
	  else {
	    cout <<"Straight Flush with rank " << tempHand.front()->rank<<endl;
	    return STRAIGHT_FLUSH;
	  }
	}
	it = tempHand.erase(it);
      }
      tempHand = pHand;
    }
  }
  else {
    if(isStraight(pHand) && isFlush(pHand)) {
      if ( getNumRank(pHand.front()->rank) == 14) {
	cout << "Royal Flush " << endl;
	return ROYAL_FLUSH;
      }
      else {
	cout << "Straight Flush with rank " <<pHand.front()->rank<< endl;
	return STRAIGHT_FLUSH;
      }
    }
  }

  if(topKind[0].size() >= 4-jokers.size()){
    cout << "Four of a kind with rank " << topKind[0].front()->rank;
    handleOverflowCards(topKind[0].size());
    printKickers(1);
    return FOUR_OF_A_KIND;
  }
  
  int remainingJokers = jokers.size()-(3-topKind[0].size());
  if(topKind[0].size() >= 3-jokers.size() && topKind[1].size() >=2-remainingJokers){
    cout << "Full house with top rank " << topKind[0].front()->rank << " and second top rank " << topKind[1].front()->rank << endl;
    return FULL_HOUSE;
  }

  if(isGreedy) {
    for(char suit:"CSDH") {
      if(isFlush(pHand, suit)) {
	cout << "Flush with rank " << pHand.front()->rank <<endl;
	return FLUSH;
      }
    }
  }
  else {
    if(isFlush(pHand)) {
      cout << "Flush with rank " <<pHand.front()->rank<<endl;
      return FLUSH;
    }
  }

  if (isGreedy) {
    list<card*> tempHand(pHand);
    for(auto it = tempHand.begin();it != tempHand.end();) {
      if(isStraight(tempHand)) {
	cout << "Straight with rank " << tempHand.front()->rank <<endl;
	return STRAIGHT;
      }
      it = tempHand.erase(it);
    }

  }
  else {
    if(isStraight(pHand)) {
      cout << "Straight with rank " <<pHand.front()->rank <<endl;
      return STRAIGHT;
    }
  }

  if(topKind[0].size() >= 3-jokers.size()) {
    cout << "Three of a kind with rank " <<topKind[0].front()->rank;
    handleOverflowCards(topKind[0].size());
    printKickers(2);
    return THREE_OF_A_KIND;
  }

  remainingJokers = jokers.size()-(2-topKind[0].size());
  if(topKind[0].size() == 2-jokers.size() && topKind[1].size() ==2-remainingJokers) {
    cout << "Two pair with rank " << topKind[0].front()->rank << " and rank " <<topKind[1].front()->rank;
    handleOverflowCards(topKind[0].size(),topKind[0].size());
    printKickers(1);
    return TWO_PAIR;
  }

  if(topKind[0].size() >= 2-jokers.size()) {
    cout << "One pair with rank " <<topKind[0].front()->rank;
    handleOverflowCards(topKind[0].size());
    printKickers(3);
    return ONE_PAIR;
  }

  cout << "High Card with " <<pHand.front()->rank;
  handleOverflowCards();
  printKickers(5);
  return HIGH_CARD;
}

/*
  Based on the hand category, redistribute overflow cards from topKind into kickers
*/
void Hand::handleOverflowCards(int first, int second) const {
  if(topKind[0].size() > first) {
    kickers.insert(kickers.begin(), topKind[0].begin()+first, topKind[0].end());
    topKind[0].erase(topKind[0].begin()+first, topKind[0].end());
  }

  if(topKind[1].size() > second) {
    kickers.insert(kickers.begin(), topKind[1].begin()+second, topKind[1].end());
    topKind[1].erase(topKind[1].begin()+second, topKind[1].end());
  }
  sort(kickers.begin(), kickers.end(), compareCards);
}

void Hand::printKickers(int count) const {
  for(int i=0; i<count; i++) {
    if(i == count-1) {
      cout << " and ";
    }
    else
      cout << ", ";
    cout << kickers[i];
  }
  cout <<" kickers"<<endl;
}
  
void Hand::findBestPermutation() const {
  findCategory(true);
  return;
}
  
bool findWinner(const Hand* hand1, const Hand* hand2) {
  categories category1 = hand1->findCategory();
  categories category2 = hand2->findCategory();

  if(category1 != category2)
    return category1 > category2;
  if(hand1->topKind[0].front()->rank != hand2->topKind[0].front()->rank)
    return hand1->topKind[0].front()->rank > hand2->topKind[0].front()->rank;
  if(hand1->topKind[1].front()->rank != hand2->topKind[1].front()->rank)
    return hand1->topKind[1].front()->rank > hand2->topKind[1].front()->rank;

  list<Hand::card*>::const_iterator it1 = hand1->pHand.begin();
  list<Hand::card*>::const_iterator it2 = hand2->pHand.begin();
  for(; it1 != hand1->pHand.end() && it2 != hand2->pHand.end();it1++,it2++) {
      
    if((*it1)->rank != (*it2)->rank)
      return (*it1)->rank > (*it2)->rank;
	
  }
  return false;
}

bool Hand::isStraight(const list<card*>& hand, char suit) const {
  list<card*>::const_iterator it = hand.begin();
  int previous = getNumRank((*it)->rank);
  int count=0;
  int joker = jokers.size();
  it++;
  for(;it!=hand.end(); it++) {
    if(previous == getNumRank((*it)->rank)+1 && (suit =='N' || suit == (*it)->suit)) {
      count++;
      previous--;
    }
    else {
      if(joker >0) {
	count++;
	previous--;
	joker--;
      }
    }
  }
  if(count >= 4)
    return true;
  else
    return false;
}

bool Hand::isFlush(const list<card*>& hand, char previous) const {
  list<card*>::const_iterator it = hand.begin();
  int count =0;
  int joker = jokers.size();
  if(previous =='N') previous = (*it)->suit;
  for(;it!=hand.end(); it++) {
    if(previous == (*it)->suit)
      count++;
    else {
      if(joker >0) {
	count++;
	joker--;
      }
    }
  }
  if(count >=5) return true;
  else return false;
}
  
int Hand::getNumRank(char c) {
  int ret;
  switch(c) {
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    ret = c-'0';
    break;
  case 'T':
    ret = 10;
    break;
  case 'J':
    ret = 11;
    break;
  case 'Q':
    ret = 12;
    break;
  case 'K':
    ret = 13;
    break;
  case 'A':
    ret = 14;
    break;
  default:
    ret = -1;
    
  }
  return ret;
}

