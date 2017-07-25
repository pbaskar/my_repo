#include<iostream>
#include<list>
#include<vector>
#include<string>

using namespace std;

enum categories { HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, STRAIGHT,
		  FLUSH, FULL_HOUSE, FOUR_OF_A_KIND, STRAIGHT_FLUSH, ROYAL_FLUSH};

class Hand {
  
  struct card {
    char rank;
    char suit;

    friend ostream& operator<<(ostream& out, const card* card) {
      out << card->rank <<card->suit;
    }

    bool isJoker() {
      return rank == 'Y';
    }
  };

public:
  static Hand* createNewHand(string input);
  categories findCategory(bool isGreedy = false) const;
  void findBestPermutation() const;

  ~Hand();

  friend bool findWinner(const Hand* hand1, const Hand* hand2);
  friend ostream& operator<<(ostream& out , const Hand* h);
  
private:
  list<card*> pHand;
  string originalHand;
  vector<card*> jokers;

  // These are derived from pHand based on hand category
  mutable vector<card*> topKind[2];
  mutable vector<card*> kickers;
  
  
  bool isFlush(const list<card*>& hand, char previous='N') const;
  bool isStraight(const list<card*>& hand, char suit='N') const;
  void printKickers(int count) const;

  void addCard(card* c);
  void preprocessHand();
  void extractJokers();
  void addJoker(card* c);
  void handleOverflowCards(int first=0, int second=0) const;
  
  static int getNumRank(char c);
  static bool compareCards(const card* c1, const card* c2);
};
