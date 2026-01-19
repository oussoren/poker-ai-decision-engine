#ifndef CARD_H
#define CARD_H
#include <string>

class Card
{
public:
    Card();
    ~Card();
    Card(int rank, int suit);
    int getRank() const;
    int getSuit() const;
    std::string toString() const;
    bool operator==(const Card& other) const;
    bool operator<(const Card& other) const;
    static std::string rankToString(int rank);
    static std::string suitToString(int suit);
    static char suitToChar(int suit);
private:
    int _rank;
    int _suit;
};

#endif // CARD_H
