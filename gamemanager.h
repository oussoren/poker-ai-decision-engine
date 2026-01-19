#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include <string>
#include "deck.h"
#include "player.h"
#include <vector>
#include "poker_info.h"
#include "gamestate.h"
#include <memory>
#include <unordered_map>
#include <chrono>
#include <random>
#include "ruleset.h"
#include "console.h"
#include <iostream>


class GameManager
{
public:
    GameManager();
    GameManager(const RuleSet& rules);
    GameManager(int smallBlind, int bigBlind, int startingChips = 1000);
    ~GameManager();
    void addPlayer(std::shared_ptr<Player> player);
    void startNewHand();
    void startNewBettingRound();
    void dealHoleCards();
    void runBettingRound();
    std::string actionToString(const PlayerAction& action);
    void handlePlayerAction(const PlayerAction& playerAct, int playerIndex);
    std::shared_ptr<Player> determineWinner(const std::vector<int>& elligiblePlayerIndices);
    void playHand();
    void playGame(int numHands);
    void advancePhase();
    const Gamestate& getGameState() const;
    void removePlayer(int playerIndex);
    int getNextActivePlayer(int currentPlayer);
    void rotateDealer();
    void updateBlindPositions();
    void setCurrentPlayerToFirstActive();
    bool isPlayerActive(int playerIndex);
    void dealFlop();
    void dealTurn();
    void dealRiver();
    void collectBlinds();
    void calculatePots();
    int getMaximumBet(int playerIndex);
    bool validateAction(const PlayerAction& action, int playerIndex);
    void collectBets();
    int getMinimumBet();
    int getMinimumRaise();
    bool isBettingRoundComplete();
    bool isHandComplete();
    bool isGameOver();
    void endHand();
    std::vector<std::shared_ptr<Player>> getActivePlayers();
    std::vector<std::shared_ptr<Player>> getPlayers();
    void distributeWinnings();
    std::vector<PlayerAction> getLegalActions(int playerIndex);
    void removeEliminatedPlayers();
    bool canMoreBettingOccur();


    //place for all the rules and game flow logic
    //inlcude things like getting the position of the next active player
private:
    RuleSet _rules;
    Gamestate _current;
    std::vector<std::shared_ptr<Player>> _players;
    Deck _deck;
    int _smallBlindAmt;
    int _bigBlindAmt;
    int _maxRaises; //max raises per betting round based on the ruleset
    int _startingChips;
    int _handNumber;
    //record betting round or just record it as a game phase???
    bool _gameActive;
    bool _handInProgress;
    // all optional stuff
    bool _anyPlayerActedThisRound;


    std::mt19937 rng; // - Random number generator for shuffling
    // std::shared_ptr<TrainingDataCollector> dataCollector; // - For ML training to be dealt with later
    // std::unordered_map<std::string, PlayerStats> playerStats; // - Track performance
    int totalHandsPlayed; // - Game statistics
    // std::chrono::time_point gameStartTime; // - For timing
};

#endif // GAMEMANAGER_H
