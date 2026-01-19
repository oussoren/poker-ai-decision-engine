#ifndef HANDSTRENGTHEVALUATOR_H
#define HANDSTRENGTHEVALUATOR_H

#include "hand.h"
#include "gamestate.h"
#include "gamemanager.h"

class HandStrengthEvaluator {
public:
    static float evaluateHandStrength(const Hand& playerHand, const GameState& gameState);

    static float evaluatePreFlopStrength(const Hand& playerHand);
    static float evaluatePostFlopStrength(const Hand& playerHand, const GameState& gameState, int numOpponents = 1);

private:
    static float getBaseStrength(int handRank);
    static float adjustForBoardTexture(float baseStrength, const GameState& gameState);
    static float adjustForOpponents(float baseStrength, int numOpponents);
    static float calculateKickerBonus(const std::vector<int>& kickers);
};

#endif
