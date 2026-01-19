#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <string>
#include "poker_info.h"
#include "card.h"
#include <vector>
#include "console.h"
#include <iostream>

class Player;


struct Pot {
    int amount;
    std::vector<int> eligiblePlayerIndices;
    Pot(int amt) : amount(amt) {}
};

class Gamestate
{
public:
    Gamestate();
    Gamestate(const std::vector<std::shared_ptr<Player>>& players);
    ~Gamestate();
    double getPotOdds() const;
    std::vector<std::shared_ptr<Player>> getActivePlayers();
    std::vector<std::shared_ptr<Player>> getPlayers() const;
    const std::vector<Card>& getCommunityCards() const;
    void addCommunityCards(Card card);
    GamePhase getCurrentPhase() const;
    int getCurrentPlayerIndex() const;
    // int getBettingRound() const;
    int getCurrentBet() const;
    void setCurrentBet(int amount);
    int getSmallBlind() const;
    int getBigBlind() const;
    int getRoundBet()  const;
    bool isPlayerActive(int index);
    int getBettingRound() const;
    void setBettingRound(int round);
    void incrementBettingRound();
    void setDealerPosition(int position);
    void setSmallBlindPosition(int position);
    void setBigBlindPosition(int position);
    void setCurrentPlayerIndex(int index);
    void setCurrentPhase(GamePhase phase);

    // Position incrementers/rotators
    void rotateDealerPosition();
    void incrementCurrentPlayer();

    // Position getters (if you don't have them)
    int getDealerPosition() const;
    int getSmallBlindPosition() const;
    int getBigBlindPosition() const;
    void reset();
    void addPot(const Pot& pot);
    const std::vector<Pot>& getPots() const;
    void clearPots();
    int getTotalPotValue() const;
private:
    std::vector<std::shared_ptr<Player>> _players;
    std::vector<Card> _communityCards;
    int _currentPlayerIndex;
    GamePhase currentPhase;
    int _currentBet;
    int _dealerPosition;
    int _smallBlindPosition;
    int _bigBlindPosition;
    int _roundBet;
    std::vector<Pot> _pots;
    int _bettingRound;
    int _smallBlind; // fiNinsoivnsovisndvoisndgoisdnfosindfosidnfosidnjfklksfbnisdufnoksdfjnsdfknsdifjn
    int _bigBlind;
    // std::vector<ActionHistory> _actionHistory;
};

#endif // GAMESTATE_H
