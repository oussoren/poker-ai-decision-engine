#include "aggrobot.h"
#include "gamemanager.h"

AggroBot::AggroBot(const std::string& name, int chips, double tightness, double aggressiveness, int position)
    : Player(name, chips, position),
    _tightness(tightness),
    _aggressiveness(aggressiveness),
    _bluffFrequency(0.2) {
}

PlayerAction AggroBot::makeDecision(const Gamestate& gameState, GameManager* gameManager) {

    float handStrength = evaluateHandStrength(gameState);

    if (handStrength < _tightness) {
        return PlayerAction(Action::fold);
    }

    if (shouldBeAggressive(gameState)) {
        return chooseAggressiveAction(gameState, gameManager);
    } else {
        return choosePassiveAction(gameState, gameManager);
    }
}

bool AggroBot::shouldBeAggressive(const Gamestate& gameState) {
    double randomRoll = (std::rand() % 100) / 100.0;

    if (randomRoll < _aggressiveness) {
        return true;
    }
    return false;
}

PlayerAction AggroBot::chooseAggressiveAction(const Gamestate& gameState, GameManager* gameManager) {
    auto legalActions = gameManager->getLegalActions(gameState.getCurrentPlayerIndex());
    int currentBet = gameState.getCurrentBet();
    float handStrength = evaluateHandStrength(gameState);


    if (currentBet == 0) {
        for (const auto& action : legalActions) {
            if (action.action == Action::bet) {
                int betSize = calculateBetSize(handStrength, gameState);
                return PlayerAction(Action::bet, betSize);
            }
        }
        return PlayerAction(Action::check);
    }

    double raiseChance = _aggressiveness * handStrength;

    double randomRoll = (rand() % 100) / 100.0;

    if (randomRoll < raiseChance) {
        for (const auto& action : legalActions) {
            if (action.action == Action::raise) {
                int raiseSize = calculateRaiseSize(currentBet, handStrength, gameState);
                return PlayerAction(Action::raise, raiseSize);
            }
        }
    }

    double callThreshold = getCallThreshold();

    if (handStrength > callThreshold) {
        for (const auto& action : legalActions) {
            if (action.action == Action::call) {
                return action;
            }
        }
    }

    return PlayerAction(Action::fold);
}

PlayerAction AggroBot::choosePassiveAction(const Gamestate& gameState, GameManager* gameManager) {
    auto legalActions = gameManager->getLegalActions(gameState.getCurrentPlayerIndex());
    int currentBet = gameState.getCurrentBet();

    // if there is an option to check always check as the preferred passive action
    for (const auto& action : legalActions) {
        if (action.action == Action::check) {
            return action;
        }
    }

    float handStrength = evaluateHandStrength(gameState);
    double callThreshold = getCallThreshold();

    if (handStrength > callThreshold) {
        for (const auto& action : legalActions) {
            if (action.action == Action::call) {
                return action;
            }
        }
    }
    return PlayerAction(Action::fold);
}

double AggroBot::getBaseStrength(int handRank) {
    switch(handRank) {
    case STRAIGHT_FLUSH: return 0.95;
    case FOUR_OF_A_KIND: return 0.90;
    case FULL_HOUSE: return 0.85;
    case FLUSH: return 0.75;
    case STRAIGHT: return 0.65;
    case THREE_OF_A_KIND: return 0.55;
    case TWO_PAIR: return 0.45;
    case ONE_PAIR: return 0.30;
    case HIGH_CARD: return 0.15;
    default: return 0.10;
    }
}

double AggroBot::getCallThreshold() {
    double tightnessComponent = 0.2 + (_tightness * 0.5);
    double aggressivenessAdjustment = _aggressiveness * 0.1;

    return tightnessComponent - aggressivenessAdjustment;
}

//adjust for preflop post flop and other factors
float AggroBot::evaluateHandStrength(const Gamestate& gameState) {
    const Hand& playerHand = this->getHand();
    return HandStrengthEvaluator::evaluateHandStrength(playerHand, gameState);
}

int AggroBot::calculateBetSize(double handStrength, const GameState& gameState) {
    int potSize = gameState.getTotalPotValue();

    if (handStrength > 0.8) {

        return potSize;
    } else if (handStrength > 0.6) {

        return potSize * 0.6;
    } else if (handStrength > 0.4) {

        return potSize * 0.3;
    } else {
        double bluffRoll = (rand() % 100) / 100.0;
        if (bluffRoll < _bluffFrequency) {
            return potSize * 0.4;
        }
        return 0;
    }
}

int AggroBot::calculateRaiseSize(int currentBet, double handStrength, const GameState& gameState) {
    if (handStrength > 0.8) {
        return currentBet * 3;
    } else if (handStrength > 0.6) {
        return currentBet * 2;
    } else {
        double bluffRoll = (rand() % 100) / 100.0;
        if (bluffRoll < _bluffFrequency) {
            return currentBet * 2;
        }
        return 0;
    }
}



double AggroBot::getPreFlopStrength(const GameState& gameState) {
    auto cards = this->getHand();
    if (cards.size() != 2) return 0.5;  // Should have exactly 2 hole cards

    int rank1 = cards[0].getRank();
    int rank2 = cards[1].getRank();
    bool suited = (cards[0].getSuit() == cards[1].getSuit());
    bool paired = (rank1 == rank2);

    // Premium hands
    if (paired && rank1 >= 10) return 0.9;  // AA, KK, QQ, JJ, TT
    if ((rank1 == 12 && rank2 == 11) || (rank1 == 11 && rank2 == 12)) return 0.85; // AK

    // Good hands
    if (paired && rank1 >= 7) return 0.7;   // 99, 88, 77
    if (suited && abs(rank1 - rank2) <= 1 && std::min(rank1, rank2) >= 9) return 0.65; // AQs, KQs, etc.

    // Mediocre hands
    if (paired) return 0.5;  // Any pair
    if (std::max(rank1, rank2) >= 11) return 0.4;  // Any ace or face card

    // Trash
    return 0.2;
}

