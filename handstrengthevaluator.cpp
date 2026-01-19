#include "handstrengthevaluator.h"

float HandStrengthEvaluator::calculateAdvancedHandStrength(const Hand& playerHand, const GameState& gameState) const {
    if (gameState.getCurrentPhase() == GamePhase::PREFLOP) {
        return calculatePreFlopStrength(playerHand);
    } else {
        return calculatePostFlopStrength(playerHand, gameState);
    }
}

float HandStrengthEvaluator::calculatePreFlopStrength(const Hand& playerHand) const {
    auto cards = playerHand.getCards();
    if (cards.size() != 2) return 0.1f;

    int rank1 = cards[0].getRank();
    int rank2 = cards[1].getRank();
    bool suited = (cards[0].getSuit() == cards[1].getSuit());
    bool paired = (rank1 == rank2);

    // Convert ranks to more intuitive scale (2=0, 3=1, ..., A=12)
    int highRank = std::max(rank1, rank2);
    int lowRank = std::min(rank1, rank2);

    // Premium pairs
    if (paired) {
        if (highRank >= 12) return 0.95f;
        if (highRank >= 11) return 0.90f;
        if (highRank >= 10) return 0.85f;
        if (highRank >= 9) return 0.75f;
        if (highRank >= 7) return 0.65f;
        if (highRank >= 5) return 0.50f;
        return 0.35f;  // Small pairs
    }

    // Big cards
    if (highRank >= 12) {  // Ace
        if (lowRank >= 11) return suited ? 0.88f : 0.82f;
        if (lowRank >= 10) return suited ? 0.78f : 0.70f;
        if (lowRank >= 9) return suited ? 0.68f : 0.60f;
        if (lowRank >= 8) return suited ? 0.58f : 0.50f;
        if (suited && lowRank >= 6) return 0.45f;
        if (suited) return 0.35f;
        if (lowRank >= 8) return 0.40f;
        return 0.25f;
    }

    if (highRank >= 11) {  // King
        if (lowRank >= 10) return suited ? 0.75f : 0.65f;
        if (lowRank >= 9) return suited ? 0.65f : 0.55f;
        if (suited && lowRank >= 8) return 0.50f;
        if (suited) return 0.35f;
        return 0.25f;
    }

    // Suited connectors and one-gappers
    if (suited) {
        int gap = highRank - lowRank;
        if (gap <= 1 && highRank >= 8) return 0.55f;      // high connector
        if (gap <= 2 && highRank >= 9) return 0.45f;      // higher connector with gaps
        if (gap <= 1) return 0.40f;                       // suited connector
        if (gap <= 3 && highRank >= 10) return 0.35f;     // Suited connector with gaps
    }

    if (highRank >= 10 && lowRank >= 9) return 0.50f;     // QJ, QT, JT

    return 0.15f;  // Trash
}

float HandStrengthEvaluator::calculatePostFlopStrength(const Hand& playerHand, const GameState& gameState) const {
    int handRank = playerHand.getHandRank();
    auto kickers = playerHand.getKickers();

    float baseStrength = getBaseStrength(handRank);

    float kickerBonus = 0.0f;
    if (!kickers.empty()) {

        kickerBonus = (float(kickers[0]) / 12.0f) * 0.15f;

        //extra good kickers
        if (kickers.size() > 1) {
            kickerBonus += (float(kickers[1]) / 12.0f) * 0.05f;
        }
    }

    float adjustedStrength = std::min(0.98f, baseStrength + kickerBonus);
    adjustedStrength = adjustForBoardTexture(adjustedStrength, gameState);
    adjustedStrength = adjustForOpponents(adjustedStrength, _numActivePlayers);

    return adjustedStrength;
}

float HandStrengthEvaluator::adjustForBoardTexture(float baseStrength, const GameState& gameState) const {
    auto communityCards = gameState.getCommunityCards();
    if (communityCards.empty()) return baseStrength;

    float dangerLevel = 0.0f;

    // check for flushdraw possibility
    std::map<int, int> suitCounts;
    for (const auto& card : communityCards) {
        suitCounts[card.getSuit()]++;
    }
    for (const auto& [suit, count] : suitCounts) {
        if (count >= 3) dangerLevel += 0.2f;
    }

    // Check for straight draws
    std::vector<int> ranks;
    for (const auto& card : communityCards) {
        ranks.push_back(card.getRank());
    }
    std::sort(ranks.begin(), ranks.end());

    int maxConnected = 1;
    int currentConnected = 1;
    for (int i = 1; i < ranks.size(); i++) {
        if (ranks[i] == ranks[i-1] + 1) {
            currentConnected++;
            maxConnected = std::max(maxConnected, currentConnected);
        } else if (ranks[i] != ranks[i-1]) {
            currentConnected = 1;
        }
    }
    if (maxConnected >= 3) dangerLevel += 0.15f;

    // Reduce strength based on danger level for medium hands
    if (baseStrength < 0.8f) {
        // doesnt punish stronger hands as much
        baseStrength *= (1.0f - dangerLevel * 0.5f);
    }

    return std::max(0.05f, baseStrength);
}

float InfoState::adjustForOpponents(float baseStrength, int numOpponents) const {
    // More opponents = need stronger hand to be good
    float opponentPenalty = (numOpponents - 1) * 0.05f;

    if (baseStrength < 0.9f) {
        baseStrength *= (1.0f - opponentPenalty);
    }

    return std::max(0.05f, baseStrength);
}

float InfoState::getBaseStrength(int handRank) const {
    switch(handRank) {
    case HIGH_CARD: return 0.05f;
    case ONE_PAIR: return 0.25f;
    case TWO_PAIR: return 0.45f;
    case THREE_OF_A_KIND: return 0.65f;
    case STRAIGHT: return 0.75f;
    case FLUSH: return 0.80f;
    case FULL_HOUSE: return 0.90f;
    case FOUR_OF_A_KIND: return 0.95f;
    case STRAIGHT_FLUSH: return 0.98f;
    default: return 0.05f;
    }
}
