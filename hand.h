#ifndef HAND_H
#define HAND_H
#include <string>
#include <vector>
#include "card.h"
#include <unordered_map>
#include <algorithm>  // For sort, unique, max, min
#include <stdexcept>

class Hand
{
public:
    Hand();
    ~Hand();
    void addCard(const Card& card);
    void addCards(const std::vector<Card>& cards);
    void clear();
    int getHandRank() const;
    std::vector<int> getKickers() const; // tie breaking stuff
    std::vector<int> getFourOfAKindKickers(const std::unordered_map<int, int>& counts) const;
    std::vector<int> getFullHouseKickers(const std::unordered_map<int, int>& counts) const;
    std::vector<int> getFlushKickers() const;
    std::vector<int> getStraightKickers() const;
    std::vector<int> getThreeOfAKindKickers(const std::unordered_map<int, int>& counts) const;
    std::vector<int> getTwoPairKickers(const std::unordered_map<int, int>& counts) const;
    std::vector<int> getOnePairKickers(const std::unordered_map<int, int>& counts) const;
    std::vector<int> getHighCardKickers() const;
    std::vector<int> getStraightFlushKickers() const;
    bool operator>(const Hand& other) const;
    std::string toString() const;
    int size();
    bool isStraight() const;
    bool isFlush() const;
    bool isOnePair() const;
    bool isTwoPair() const;
    bool isThreeOfAKind() const;
    bool isFullHouse() const;
    bool isFourOfAKind() const;
    bool isStraightFlush() const;
    std::unordered_map<int, int> getRankCounts() const;
private:
    std::vector<Card> _cards;
};

#endif // HAND_H
