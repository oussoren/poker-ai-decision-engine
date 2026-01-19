#ifndef RULESET_H
#define RULESET_H

enum class BettingStructure {
    NO_LIMIT,
    POT_LIMIT,
    FIXED_LIMIT
};

enum class GameType {
    CASH_GAME,
    TOURNAMENT,
    SIT_AND_GO
};

class RuleSet
{
private:
    // Basic game settings
    int _smallBlind;
    int _bigBlind;
    int _startingChips;
    int _maxRaises;

    // Betting structure
    BettingStructure _bettingType;  // NO_LIMIT, POT_LIMIT, FIXED_LIMIT

    // Tournament vs Cash game
    GameType _gameType;

    // Other rules
    bool _allowStringBets;
    bool _allowCheckRaise;
    int _minPlayers;
    int _maxPlayers;

public:
    // Constructors for different game types
    RuleSet();  // Default constructor
    RuleSet(int smallBlind, int bigBlind, int startingChips, int maxRaises = 3);
    static RuleSet createTexasHoldem();
    static RuleSet createTournament(int startingChips);
    static RuleSet createCashGame(int smallBlind, int bigBlind);
    static RuleSet createPotLimit(int smallBlind, int bigBlind);
    static RuleSet createFixedLimit(int smallBlind, int bigBlind);

    // Getters
    int getSmallBlind() const { return _smallBlind; }
    int getBigBlind() const { return _bigBlind; }
    int getStartingChips() const { return _startingChips; }
    int getMaxRaises() const { return _maxRaises; }
    BettingStructure getBettingType() const { return _bettingType; }

    // Rule queries
    bool isValidBet(int amount, int currentBet, int playerChips) const;
    bool canCheckRaise() const { return _allowCheckRaise; }
    int getMinimumRaise(int currentBet) const;

    int getMaximumBet(int currentBet, int playerChips, int potSize) const;
    bool isValidPlayerCount(int numPlayers) const;
    GameType getGameType() const { return _gameType; }
    bool allowStringBets() const { return _allowStringBets; }
    int getMinPlayers() const { return _minPlayers; }
    int getMaxPlayers() const { return _maxPlayers; }
};

#endif // RULESET_H
