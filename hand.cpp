#include "hand.h"
#include "card.h"
#include <unordered_map>
#include <random>
#include <vector>

using namespace std;

enum HandRank {
    HIGH_CARD = 1,
    ONE_PAIR = 2,
    TWO_PAIR = 3,
    THREE_OF_A_KIND = 4,
    STRAIGHT = 5,
    FLUSH = 6,
    FULL_HOUSE = 7,
    FOUR_OF_A_KIND = 8,
    STRAIGHT_FLUSH = 9
};

Hand::Hand() {}

Hand::~Hand(){}

void Hand::addCard(const Card& card) {
    if (_cards.size() >= 7) throw runtime_error("Hand full");
    _cards.push_back(card);
}

void Hand::addCards(const vector<Card>& cards){
    for (const Card& kard: cards) {
        if (_cards.size() >= 7) throw runtime_error("Hand full");
        _cards.push_back(kard);
    }
}

void Hand::clear(){
    _cards.clear();
}

int Hand::getHandRank() const{
    if (isStraightFlush()) return STRAIGHT_FLUSH;
    if (isFourOfAKind()) return FOUR_OF_A_KIND;
    if (isFullHouse()) return FULL_HOUSE;
    if (isFlush()) return FLUSH;
    if (isStraight()) return STRAIGHT;
    if (isThreeOfAKind()) return THREE_OF_A_KIND;
    if (isTwoPair()) return TWO_PAIR;
    if (isOnePair()) return ONE_PAIR;
    return HIGH_CARD;
}

vector<int> Hand::getKickers() const{
    int rank = getHandRank();
    std::unordered_map<int, int> counts = getRankCounts();
    vector<int> kickers;

    switch (rank) {
    case FOUR_OF_A_KIND:
        return getFourOfAKindKickers(counts);
    case FULL_HOUSE:
        return getFullHouseKickers(counts);
    case STRAIGHT_FLUSH:
        return getStraightFlushKickers();
    case FLUSH:
        return getFlushKickers();
    case STRAIGHT:
        return getStraightKickers();
    case THREE_OF_A_KIND:
        return getThreeOfAKindKickers(counts);
    case TWO_PAIR:
        return getTwoPairKickers(counts);
    case ONE_PAIR:
        return getOnePairKickers(counts);
    case HIGH_CARD:
        return getHighCardKickers();
    default:
        return kickers;
    }
}

vector<int> Hand::getStraightFlushKickers() const {
    // Group cards by suit
    unordered_map<int, vector<Card>> suitGroups;
    for (const Card& card : _cards) {
        suitGroups[card.getSuit()].push_back(card);
    }

    // Find the suit with the straight flush
    for (auto& [suit, suitCards] : suitGroups) {
        if (suitCards.size() >= 5) {
            // Sort this suit's cards
            sort(suitCards.begin(), suitCards.end(), [](const Card& a, const Card& b) {
                return a.getRank() < b.getRank();
            });

            // Check for straight within this suit
            int consecutive = 1;
            int highCard = suitCards[0].getRank();

            for (int i = 1; i < suitCards.size(); i++) {
                if (suitCards[i].getRank() == suitCards[i-1].getRank() + 1) {
                    consecutive++;
                    if (consecutive >= 5) {
                        highCard = suitCards[i].getRank();  // Current high card
                        if (i == suitCards.size() - 1 ||
                            suitCards[i+1].getRank() != suitCards[i].getRank() + 1) {
                            // End of straight, return high card
                            return {highCard};
                        }
                    }
                } else {
                    consecutive = 1;
                    highCard = suitCards[i].getRank();
                }
            }

            // Check wheel straight flush (A-2-3-4-5)
            if (suitCards.size() >= 5 && suitCards.back().getRank() == 12 &&
                suitCards[0].getRank() == 0 && suitCards[1].getRank() == 1 &&
                suitCards[2].getRank() == 2 && suitCards[3].getRank() == 3) {
                return {3};  // 5 is high card in wheel straight
            }
        }
    }

    return {};  // Shouldn't reach here if isStraightFlush() returned true
}

vector<int> Hand::getFlushKickers() const {
    // Find the suit with 5+ cards
    unordered_map<int, vector<int>> suitRanks;
    for (const Card& card : _cards) {
        suitRanks[card.getSuit()].push_back(card.getRank());
    }

    // Find flush suit and get its ranks
    vector<int> flushRanks;
    for (auto& [suit, ranks] : suitRanks) {
        if (ranks.size() >= 5) {
            flushRanks = ranks;
            break;
        }
    }

    // Sort high to low, take top 5
    sort(flushRanks.rbegin(), flushRanks.rend());
    return vector<int>(flushRanks.begin(), flushRanks.begin() + 5);
}

vector<int> Hand::getStraightKickers() const {
    vector<int> ranks;
    for (const Card& card : _cards) {
        ranks.push_back(card.getRank());
    }

    // Remove duplicates and sort
    sort(ranks.begin(), ranks.end());
    ranks.erase(unique(ranks.begin(), ranks.end()), ranks.end());

    // Find the highest straight
    int straightHigh = -1;

    // Check regular straights (5-high through Ace-high)
    for (int i = ranks.size() - 1; i >= 4; i--) {
        if (ranks[i] == ranks[i-1] + 1 && ranks[i-1] == ranks[i-2] + 1 &&
            ranks[i-2] == ranks[i-3] + 1 && ranks[i-3] == ranks[i-4] + 1) {
            straightHigh = ranks[i];
            break;
        }
    }

    // Check wheel straight (A-2-3-4-5)
    if (straightHigh == -1 && ranks.size() >= 5) {
        if (ranks.back() == 12 && ranks[0] == 0 && ranks[1] == 1 &&
            ranks[2] == 2 && ranks[3] == 3) {
            straightHigh = 5;  // 5-high straight (wheel)
        }
    }

    return {straightHigh};  // Only one kicker - the high card
}

vector<int> Hand::getFourOfAKindKickers(const unordered_map<int, int>& counts) const {
    vector<int> kickers;
    int quadRank = -1;
    int kickerRank = -1;
    for (const auto& [rank, count] : counts) {
        if (count == 4) quadRank = rank;
        else if (count >= 1) kickerRank = max(kickerRank, rank);
    }

    kickers.push_back(quadRank);   // Quad rank first
    kickers.push_back(kickerRank); // Highest remaining card
    return kickers;
}

vector<int> Hand::getThreeOfAKindKickers(const unordered_map<int, int>& counts) const {
    vector<int> kickers;
    int tripRank = -1;
    int kickerRank = -1;
    for (const auto& [rank, count] : counts) {
        if (count == 3) tripRank = rank;
        else if (count >= 1) kickerRank = max(kickerRank, rank);
    }

    kickers.push_back(tripRank);   // Quad rank first
    kickers.push_back(kickerRank); // Highest remaining card
    return kickers;
}

vector<int> Hand::getFullHouseKickers(const unordered_map<int, int>& counts) const {
    vector<int> kickers;
    int tripRank = -1, pairRank = -1;

    for (const auto& [rank, count] : counts) {
        if (count == 3) tripRank = max(tripRank, rank);  // Highest trips
        if (count == 2) pairRank = max(pairRank, rank);   // Highest pair
    }

    kickers.push_back(tripRank);  // Trips first
    kickers.push_back(pairRank);  // Pair second
    return kickers;
}

vector<int> Hand::getTwoPairKickers(const unordered_map<int, int>& counts) const {
    vector<int> kickers;
    vector<int> pairs;
    vector<int> singles;
    for (const auto& [rank, count] : counts) {
        if (count == 2) pairs.push_back(rank);
        else if (count == 1) singles.push_back(rank);
    }

    sort(pairs.rbegin(), pairs.rend());

    // Add in order: higher pair, lower pair, kicker
    kickers.push_back(pairs[0]);  // Higher pair
    kickers.push_back(pairs[1]);  // Lower pair
    if (!singles.empty()) {
        sort(singles.rbegin(), singles.rend());
        kickers.push_back(singles[0]);
    }

    return kickers;
}

vector<int> Hand::getOnePairKickers(const unordered_map<int, int>& counts) const {
    vector<int> kickers;
    int pairRank = -1;
    vector<int> singleCards;
    for (const auto& [rank, count] : counts) {
        if (count == 2) pairRank = rank;
        else if (count == 1) singleCards.push_back(rank);
    }

    // Sort single cards high to low
    sort(singleCards.rbegin(), singleCards.rend());

    kickers.push_back(pairRank);  // Pair rank first
    // Add up to 3 highest single cards
    for (int i = 0; i < min(3, (int)singleCards.size()); i++) {
        kickers.push_back(singleCards[i]);
    }

    return kickers;
}

vector<int> Hand::getHighCardKickers() const {
    vector<int> ranks;
    for (const Card& card : _cards) {
        ranks.push_back(card.getRank());
    }

    // Sort high to low, take top 5
    sort(ranks.rbegin(), ranks.rend());
    return vector<int>(ranks.begin(), ranks.begin() + min(5, (int)ranks.size()));
}

string Hand::toString() const{
    string result;
    for (Card kard: _cards) {
        result += kard.rankToString(kard.getRank());
        result += kard.suitToString(kard.getSuit());
    }
    return result;
}

int Hand::size(){
    return _cards.size();
}

bool Hand::isStraight() const{
    vector<Card> sortedCards = _cards;
    sort(sortedCards.begin(), sortedCards.end(), [](const Card& a, const Card& b) {
        return a.getRank() < b.getRank();  // Low to high
    });
    int consecutive = 1;
    for (int i = 1; i < sortedCards.size(); i++) {
        if (sortedCards[i].getRank() == sortedCards[i-1].getRank()) {
            continue;
        }
        else if (sortedCards[i].getRank() == sortedCards[i-1].getRank() + 1) {
            consecutive++;
            if (consecutive >= 5) return true;
        } else {
            consecutive = 1;
        }
    }

    if (sortedCards.back().getRank() == 12 && sortedCards[0].getRank() == 0 &&
        sortedCards[1].getRank() == 1 && sortedCards[2].getRank() == 2 &&
        sortedCards[3].getRank() == 3) {
        return true;
    }

    return false;
}

bool Hand::isFlush() const{
    unordered_map<char, int> counts;
    for (const Card& card: _cards) {
        counts[card.suitToChar(card.getSuit())]++;
    }
    string suits = "cdhs";
    for (const auto& [suit, count] : counts) {
        if (count >= 5) {
            return true;
        }
    }
    return false;
}

bool Hand::isOnePair() const {
    unordered_map counts = getRankCounts();
    for (const auto& [rank, count] : counts) {
        if (count == 2) return true;
    }
    return false;
}
bool Hand::isTwoPair() const {
    unordered_map counts = getRankCounts();
    int pairs = 0;
    for (const auto& [rank, count] : counts) {
        if (count == 2) {
            pairs++;
        }
    }
    return pairs >= 2;
}

bool Hand::isThreeOfAKind() const {
    unordered_map counts = getRankCounts();
    for (const auto& [rank, count] : counts) {
        if (count == 3) return true;
    }
    return false;
}

bool Hand::isFullHouse() const {
    unordered_map counts = getRankCounts();
    bool isPair = false;
    bool isThree = false;
    for (const auto& [rank, count] : counts) {
        if (count == 2) {
            isPair = true;
        }
        if (count == 3) {
            isThree = true;
        }
    }

    return isPair && isThree;
}

bool Hand::isFourOfAKind() const {
    unordered_map counts = getRankCounts();
    for (const auto& [rank, count] : counts) {
        if (count == 4) return true;
    }
    return false;
}

bool Hand::isStraightFlush() const {
    if (!isStraight() || !isFlush()) return false;

    unordered_map<int, vector<Card>> suitGroups;
    for (const Card& card : _cards) {
        suitGroups[card.getSuit()].push_back(card);
    }

    for (auto& [suit, suitCards] : suitGroups) {
        if (suitCards.size() >= 5) {

            sort(suitCards.begin(), suitCards.end(), [](const Card& a, const Card& b) {
                return a.getRank() < b.getRank();
            });


            int consecutive = 1;
            for (int i = 1; i < suitCards.size(); i++) {
                if (suitCards[i].getRank() == suitCards[i-1].getRank() + 1) {
                    consecutive++;
                    if (consecutive >= 5) return true;
                } else {
                    consecutive = 1;
                }
            }

            if (suitCards.size() >= 5 && suitCards.back().getRank() == 12 &&
                suitCards[0].getRank() == 0 && suitCards[1].getRank() == 1 &&
                suitCards[2].getRank() == 2 && suitCards[3].getRank() == 3) {
                return true;
            }
        }
    }

    return false;
}


bool Hand::operator>(const Hand& other) const {
    int thisRank = this->getHandRank();
    int otherRank = other.getHandRank();

    // Compare hand ranks first
    if (thisRank != otherRank) {
        return thisRank > otherRank;
    }

    // Same rank - compare kickers
    std::vector<int> thisKickers = this->getKickers();
    std::vector<int> otherKickers = other.getKickers();

    return thisKickers > otherKickers;  // Lexicographic comparison
}

unordered_map<int, int> Hand::getRankCounts() const{
    unordered_map<int, int> results;
    for (int i = 0; i < _cards.size(); i++) {
        results[_cards[i].getRank()]++;
    }
    return results;
}
