#ifndef BALANCEDBOT_H
#define BALANCEDBOT_H
#include "player.h"


class GameManager;

class BalancedBot : public Player {

public:
    BalancedBot(const std::string& name, int chips, double tightness = 0.6,
                double aggressiveness = 0.5, int position = -1);
    PlayerAction makeDecision(const Gamestate& gameState, GameManager* gameManager);

private:
    double _tightness;
    double _aggressiveness;
    double _bluffFrequency;

    double evaluateHandStrength(const Gamestate& gameState);
    PlayerAction chooseAggressiveAction(const Gamestate& gameState, GameManager* gm);
    PlayerAction choosePassiveAction(const Gamestate& gameState, GameManager* gm);
    int calculateBetSize(double handStrength, int potSize);
};

#endif
