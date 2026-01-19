#ifndef TIGHTBOT_H
#define TIGHTBOT_H
#include "player.h"

class GameManager;

class TightBot : public Player {

public:
    TightBot(const std::string& name, int chips, double tightness = 0.8,
             double aggressiveness = 0.3, int position = -1);
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
