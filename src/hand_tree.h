#ifndef _HAND_TREE_H_
#define _HAND_TREE_H_

#include "cards.h"

class CanonicalCards;

class HandTree {
public:
  HandTree(int root_st, int root_bd, int final_st);
  ~HandTree(void);
  const CanonicalCards *Hands(int st, int lbd) const {
    return hands_[st][lbd];
  }
private:
  int root_st_;
  int root_bd_;
  int final_st_;
  CanonicalCards ***hands_;
};

int HCPIndex(int st, const Card *cards);

#endif