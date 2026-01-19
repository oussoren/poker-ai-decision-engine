#include "gamehistory.h"

// DecisionOutcome constructor
GameHistory::decisionRecord::decisionRecord(const InfoState& s, const PlayerAction& a, int outcome)
    : state(s), actionTaken(a), chipsWon(outcome) {
}

// GameHistory constructor
GameHistory::GameHistory() {
    //set aside space for more completed decisions and some for current decisions
    _completedDecisions.reserve(1000);
    _decisionsInProgress.reserve(10);
}

void GameHistory::recordDecision(const InfoState& state, const PlayerAction& action) {
    _decisionsInProgress.emplace_back(state, action, 0);
}

void GameHistory::recordOutcome(int totalChipsWon) {
    // Distribute the outcome across all decisions in this hand
    int outcomePerDecision = _decisionsInProgress.empty() ? 0 :
                                 totalChipsWon / static_cast<int>(_decisionsInProgress.size());

    for (auto& decision : _decisionsInProgress) {
        decision.chipsWon = outcomePerDecision;
        _completedDecisions.push_back(decision);
    }

    _decisionsInProgress.clear();
}

std::pair<std::vector<std::vector<float>>, std::vector<float>> GameHistory::getTrainingData() const {
    std::vector<std::vector<float>> features;
    std::vector<float> outcomes;

    // Reserve space for efficiency
    features.reserve(_completedDecisions.size());
    outcomes.reserve(_completedDecisions.size());

    for (const auto& decision : _completedDecisions) {
        features.push_back(decision.state.getFullFeatures(decision.actionTaken));
        outcomes.push_back(static_cast<float>(decision.chipsWon));  // Cast to float for ML
    }

    return {features, outcomes};
}

size_t GameHistory::getCompletedDecisionCount() const {
    return _completedDecisions.size();
}

void GameHistory::clearAllData() {
    _completedDecisions.clear();
    _decisionsInProgress.clear();
}

void GameHistory::clearOldData(size_t keepLastN) {
    if (_completedDecisions.size() > keepLastN) {
        // Keep only the last keepLastN decisions
        _completedDecisions.erase(
            _completedDecisions.begin(),
            _completedDecisions.begin() + (_completedDecisions.size() - keepLastN)
            );
    }
}
