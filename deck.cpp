#include "deck.h"
#include <random>
#include <algorithm>
#include <vector>

using namespace std;


Deck::Deck() {
    _currentCard = 0;
    for (int suit = 0; suit < 4; suit++) {        // 4 suits
        for (int rank = 0; rank < 13; rank++) {
            _cards.push_back(Card(rank, suit));
        }
    }
    shuffle();
}

Deck::~Deck() {

}

void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(_cards.begin(), _cards.end(), gen);

    _currentCard = 0;
}

Card Deck::deal() {
    Card val = _cards[_currentCard];
    _currentCard++;
    return val;
}

void Deck::reset() {
    shuffle();
    _currentCard = 0;
}

bool Deck::isEmpty() const {
    return _currentCard >= _cards.size();
}

int Deck::cardsRemaining() const {
    return 52 - _currentCard;
}

void Deck::addCard(const Card& card) {
    _cards.push_back(card);
}
