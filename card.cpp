#include "card.h"
#include <random>


using namespace std;

Card::Card() {
    _rank = rand() % (13);
    _suit = rand() % 4;
}

Card::Card(int rank, int suit) {
    _rank = rank;
    _suit = suit;
}

Card::~Card() {

}

int Card::getRank() const {
    return _rank;
}

int Card::getSuit() const {
    return _suit;
}

string Card::toString() const {
    return rankToString(_rank) + suitToString(_suit);
}

bool Card::operator==(const Card& other) const {
    return (_rank == other._rank);
}

bool Card::operator<(const Card& other) const {
    return _rank < other._rank;
}

string Card::rankToString(int rank) {
    switch(rank) {
        case 0: return "2";
        case 1: return "3";
        case 2: return "4";
        case 3: return "5";
        case 4: return "6";
        case 5: return "7";
        case 6: return "8";
        case 7: return "9";
        case 8: return "10";
        case 9: return "J";
        case 10: return "Q";
        case 11: return "K";
        case 12: return "A";
        default: return "?";
    }
}

string Card::suitToString(int suit) {
    switch(suit) {
        case 0: return "c";
        case 1: return "d";
        case 2: return "s";
        case 3: return "h";
        default: return "?";
    }
}

char Card::suitToChar(int suit) {
    switch(suit) {
        case 0: return 'c';
        case 1: return 'd';
        case 2: return 's';
        case 3: return 'h';
        default: return '?';
    }
}
