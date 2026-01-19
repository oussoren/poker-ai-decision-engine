#ifndef DECK_H
#define DECK_H
#include <string>
#include "card.h"
#include <vector>

class Deck
{
public:
    Deck();
    ~Deck();
    void shuffle();
    Card deal();
    void reset();
    bool isEmpty() const;
    int cardsRemaining() const;
    void addCard(const Card& card);
private:
    std::vector<Card> _cards;
    int _currentCard;
};

#endif // DECK_H
