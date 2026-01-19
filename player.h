#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "poker_info.h"
#include "hand.h"
#include "handstrengthevaluator.h"

class Gamestate;

class GameManager;

class Player
{
public:
    Player(const std::string& name, int chips, int position = -1);
    ~Player();
    virtual PlayerAction makeDecision(const Gamestate& gameState, GameManager* gameManager) = 0;
    bool hasActed() const;
    bool canAct() const;
    bool canCall(int amount);
    bool canRaise(int currentBet, int minRaise);
    bool canCheck(int currentBet);
    bool isFolded();
    bool isAllIn();
    bool hasActed();
    bool canWinMainPot();
    bool canWinSidePot();
    int getMaxBet() const;
    void fold();
    void setBet(int amount);
    void addToBet(int amount);
    void check();
    void goAllIn();
    void clearHand();
    void dealtCard(const Card& card);
    void clearBet();
    void dealtCards(const std::vector<Card>& cards);
    void resetCards();
    std::string getName();
    int getPosition();
    int getChips();
    const Hand& getHand() const;
    int getAction();
    int getRoundBet();
    void setRoundBet(int amount);
    void clearRoundBet();
    int getTotalBet();
    void addChips(int amount);
    bool deductChips(int amount);
    bool hasEnoughChips(int amount);
    int evaluateHand() const;
    virtual void reset();                         // Clear folded, allIn, currentBet, hand
private:
    std::string _name;
    int _position;
    int _chips;
    int _roundBet;
    int _totalBet;
    int _action; //-1 = nothing, 0 = folded,  1 = all in, 2 = active, 3 = sitting out
    Hand _cards;

};


#endif // PLAYER_H
