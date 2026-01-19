#ifndef INFOSTATE_H
#define INFOSTATE_H
#include <vector>
#include <string>
#include "gamestate.h"
#include "player.h"
#include "poker_info.h"
#include "gamemanager.h"
#include "handstrengthevaluator.h"

class InfoState
{
public:
    InfoState(const GameState& gameState, int playerIndex);
    ~InfoState() = default;
    // for copying and assignment
    InfoState(const InfoState& other) = default;
    InfoState& operator=(const InfoState& other) = default;
    std::vector<float> getBaseFeatures() const;
    std::vector<float> getActionFeatures(const PlayerAction& action) const;
    std::vector<float> getFullFeatures(const PlayerAction& action) const;
    float getStackToTotalRatio() const { return _stackToTotalRatio; }
    float getHandStrength() const { return _handStrength; }
    float getPlayerDensity() const { return _playerDensity; }
    float getGamePhase() const { return _gamePhase; }
    float getPosition() const { return _position; }
    int getPotSize() const { return _potSize; }
    int getCurrentBet() const { return _currentBet; }
    int getYourCurrentBet() const { return _yourCurrentBet; }
    int getYourChips() const { return _yourChips; }
    float calculatePotOdds() const;
    float calculateStackToPotRatio() const;

private:
    float _stackToTotalRatio;
    float _handStrength;
    float _playerScale; // how many active players : max num players
    float _gamePhase;
    float _bettingRound;
    float _position;
    int _potSize;
    int _currentBet;
    int _playerCurrentBet;
    int _playerChips;
    int _activePlayers;

    float normalizeGamePhase(GamePhase phase) const;
    float normalizePosition(int playerIndex, int dealerPos, int numPlayers) const;

    // for more complicated feature encoding
    // float estimateOpponentTightness() const;
    // float calculateImpliedOdds() const;
    // float calculateFoldEquity(int betSize) const;
    // float getBetBluffPotential(int betSize) const;

    std::vector<float> getFoldFeatures() const;
    std::vector<float> getCallFeatures() const;
    std::vector<float> getBetFeatures(const PlayerAction& action) const;
    std::vector<float> getRaiseFeatures(const PlayerAction& action) const;

    bool isValidPlayerIndex(const GameState& gameState, int playerIndex) const;
    void validateFeatures() const;
};

#endif // INFOSTATE_H
