#ifndef RANDOMBOT_H
#define RANDOMBOT_H
#include "player.h"

class GameManager;

class RandomBot : public Player {
public:
    RandomBot(const std::string& name, int chips, int position = -1);
    PlayerAction makeDecision(const Gamestate& gameState, GameManager* gameManager);  // Must match exactly
};

#endif
