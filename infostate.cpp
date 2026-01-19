#include "infostate.h"

InfoState::InfoState(const GameState& gameState, int playerIndex) {
    auto player = gameState.getPlayers()[playerIndex];
    auto allPlayers = gameState.getPlayers();

    // calculate STR
    int totalChips = 0;
    int activePlayers = 0;
    for (const auto& p : allPlayers) {
        totalChips += p->getChips();
        if (p->canAct()) activePlayers++;
    }
    _stackToTotalRatio = float(player->getChips()) / float(totalChips);

    //evaluate hand
    _handStrength = calculateAdvancedHandStrength(playerHand, gameState);

    _playerScale = float(activePlayers) / float(allPlayers.size());

    _position = normalizePosition(playerIndex, gameState.getDealerPosition(), allPlayers.size());
    _potSize = gameState.getTotalPotValue();
    _currentBet = gameState.getCurrentBet();
    _yourCurrentBet = player->getRoundBet();
}

float InfoState::normalizeGamePhase(GamePhase phase) const {
    switch(phase) {
    case GamePhase::preflop: return 0.0f;
    case GamePhase::flop: return 0.33f;
    case GamePhase::turn: return 0.66f;
    case GamePhase::river: return 1.0f;
    default: return 0.0f;
    }
}

float InfoState::normalizePosition(int playerIndex, int dealerPos, int numPlayers) const {
    int seatsFromDealer = (playerIndex - dealerPos + numPlayers) % numPlayers;
    return float(seatsFromDealer) / float(numPlayers - 1);
}

float InfoState::calculatePotOdds() const {
    if (_currentBet <= _yourCurrentBet) return 0.0f;  // No bet to call

    int callAmount = _currentBet - _yourCurrentBet;
    return float(callAmount) / float(_potSize + callAmount);
}

std::vector<float> InfoState::getBaseFeatures() const {
    return {
        _stackToTotalRatio,
        _handStrength,
        _playerDensity,
        _gamePhase,
        _position,
        calculatePotOdds()
    };
}

std::vector<float> InfoState::getActionFeatures(const PlayerAction& action) const {
    std::vector<float> actionFeatures;

    switch(action.action) {
    case Action::fold:
        actionFeatures.push_back(0.0f);  // Risk level
        actionFeatures.push_back(float(_yourCurrentBet) / float(std::max(1, _potSize)));  // Sunk cost
        actionFeatures.push_back(0.0f);  // Aggression level
        break;

    case Action::call:
        int callAmount = std::max(0, _currentBet - _yourCurrentBet);
        actionFeatures.push_back(0.3f);  // Risk level
        actionFeatures.push_back(float(callAmount) / float(_potSize + callAmount));  // Investment ratio
        actionFeatures.push_back(0.3f);  // Aggression level
        break;

    case Action::bet:
    case Action::raise:
        actionFeatures.push_back(0.8f);  // Risk level
        actionFeatures.push_back(float(action.amount) / float(_potSize));  // Bet sizing
        actionFeatures.push_back(action.action == Action::bet ? 0.7f : 1.0f);  // Aggression
        break;
    }

    return actionFeatures;
}

std::vector<float> InfoState::getFullFeatures(const PlayerAction& action) const {
    std::vector<float> features = getBaseFeatures();
    std::vector<float> actionFeatures = getActionFeatures(action);

    // add the specific action features to the end of the base features vector
    features.insert(features.end(), actionFeatures.begin(), actionFeatures.end());
    return features;
}
