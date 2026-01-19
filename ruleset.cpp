
// ruleset.cpp
#include "ruleset.h"
#include <algorithm>

// Default constructor - creates standard Texas Hold'em rules
RuleSet::RuleSet()
    : _smallBlind(1),
    _bigBlind(2),
    _startingChips(1000),
    _maxRaises(3),
    _bettingType(BettingStructure::NO_LIMIT),
    _gameType(GameType::CASH_GAME),
    _allowStringBets(false),
    _allowCheckRaise(true),
    _minPlayers(2),
    _maxPlayers(10) {
}

// Custom constructor
RuleSet::RuleSet(int smallBlind, int bigBlind, int startingChips, int maxRaises)
    : _smallBlind(smallBlind),
    _bigBlind(bigBlind),
    _startingChips(startingChips),
    _maxRaises(maxRaises),
    _bettingType(BettingStructure::NO_LIMIT),
    _gameType(GameType::CASH_GAME),
    _allowStringBets(false),
    _allowCheckRaise(true),
    _minPlayers(2),
    _maxPlayers(10) {
}

// Static factory methods
RuleSet RuleSet::createTexasHoldem() {
    RuleSet rules;
    rules._smallBlind = 1;
    rules._bigBlind = 2;
    rules._startingChips = 1000;
    rules._maxRaises = 3;
    rules._bettingType = BettingStructure::NO_LIMIT;
    rules._gameType = GameType::CASH_GAME;
    rules._allowCheckRaise = true;
    rules._allowStringBets = false;
    rules._minPlayers = 2;
    rules._maxPlayers = 10;
    return rules;
}

RuleSet RuleSet::createTournament(int startingChips) {
    RuleSet rules;
    rules._smallBlind = 10;
    rules._bigBlind = 20;
    rules._startingChips = startingChips;
    rules._maxRaises = 3;
    rules._bettingType = BettingStructure::NO_LIMIT;
    rules._gameType = GameType::TOURNAMENT;
    rules._allowCheckRaise = true;
    rules._allowStringBets = false;
    rules._minPlayers = 2;
    rules._maxPlayers = 10;
    return rules;
}

RuleSet RuleSet::createCashGame(int smallBlind, int bigBlind) {
    RuleSet rules;
    rules._smallBlind = smallBlind;
    rules._bigBlind = bigBlind;
    rules._startingChips = bigBlind * 100; // 100 big blinds default
    rules._maxRaises = 3;
    rules._bettingType = BettingStructure::NO_LIMIT;
    rules._gameType = GameType::CASH_GAME;
    rules._allowCheckRaise = true;
    rules._allowStringBets = false;
    rules._minPlayers = 2;
    rules._maxPlayers = 10;
    return rules;
}

RuleSet RuleSet::createPotLimit(int smallBlind, int bigBlind) {
    RuleSet rules = createCashGame(smallBlind, bigBlind);
    rules._bettingType = BettingStructure::POT_LIMIT;
    return rules;
}

RuleSet RuleSet::createFixedLimit(int smallBlind, int bigBlind) {
    RuleSet rules = createCashGame(smallBlind, bigBlind);
    rules._bettingType = BettingStructure::FIXED_LIMIT;
    rules._maxRaises = 4; // Typically allow more raises in fixed limit
    return rules;
}

// Rule validation methods
bool RuleSet::isValidBet(int amount, int currentBet, int playerChips) const {
    if (amount < 0 || amount > playerChips) {
        return false;
    }

    switch (_bettingType) {
    case BettingStructure::NO_LIMIT:
        // In no-limit, any amount up to all chips is valid
        return amount >= getMinimumRaise(currentBet) || amount == playerChips;

    case BettingStructure::FIXED_LIMIT:
        // In fixed limit, only specific bet sizes allowed
        return amount == _bigBlind || amount == currentBet + _bigBlind;

    case BettingStructure::POT_LIMIT:
        // In pot limit, maximum bet is pot size
        // This would need pot size parameter - simplified for now
        return amount >= getMinimumRaise(currentBet);
    }

    return false;
}

int RuleSet::getMinimumRaise(int currentBet) const {
    switch (_bettingType) {
    case BettingStructure::NO_LIMIT:
        return currentBet + _bigBlind;

    case BettingStructure::FIXED_LIMIT:
        return currentBet + _bigBlind;

    case BettingStructure::POT_LIMIT:
        return currentBet + _bigBlind;
    }

    return currentBet + _bigBlind;
}

int RuleSet::getMaximumBet(int currentBet, int playerChips, int potSize) const {
    switch (_bettingType) {
    case BettingStructure::NO_LIMIT:
        return playerChips; // Can bet all chips

    case BettingStructure::FIXED_LIMIT:
        return currentBet + _bigBlind; // Fixed increment

    case BettingStructure::POT_LIMIT:
        return std::min(playerChips, currentBet + potSize); // Pot-sized bet
    }

    return playerChips;
}

bool RuleSet::isValidPlayerCount(int numPlayers) const {
    return numPlayers >= _minPlayers && numPlayers <= _maxPlayers;
}
