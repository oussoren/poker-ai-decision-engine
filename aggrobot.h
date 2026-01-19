#ifndef AGGROBOT_H
#define AGGROBOT_H
#include "player.h"

class GameManager;

class AggroBot : public Player {

public:
    AggroBot(const std::string& name, int chips, double tightness = 0.4,
             double aggressiveness = 0.8, int position = -1);
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
