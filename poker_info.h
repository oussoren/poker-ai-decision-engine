#ifndef POKER_INFO_H
#define POKER_INFO_H
#include <string>

enum class Action { fold, check, call, bet, raise, all_in};
enum class GamePhase { preflop, flop, turn, river, showdown};

struct PlayerAction {
    Action actionType;
    int amount;
    double confidence; // to be used later for ML seems to be important from what I've seen online
    std::string reasoning;

    PlayerAction(Action action) : actionType(action), amount(0) {}

    PlayerAction(Action action, int amt) : actionType(action), amount(amt) {}
};

#endif // POKER_INFO_H
