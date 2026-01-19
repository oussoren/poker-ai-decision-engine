#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H
#include "infostate.h"

class GameHistory
{
public:
    GameHistory();

    ~GameHistory() = default;

    void recordDecision(const InfoState& state, const PlayerAction& action);

    void recordOutcome(int totalChipsWon);

    std::pair<std::vector<std::vector<float>>, std::vector<float>> getTrainingData() const;

    size_t getCompletedDecisionCount() const;

    void clearAllData();

    void clearOldData(size_t keepLastN = 10000);
private:
    struct decisionRecord {
        InfoState state;
        PlayerAction action;
        int chipsWon;  // associated outcome

        decisionRecord(const InfoState& state, const PlayerAction& action, int hand);
        std::vector<decisionRecord> _pendingDecisions; //tracking the current decisions which have not yet been associated with an outcome
    };
    std::vector<decisionRecord> _completedDecisions;
    std::vector<decisionRecord> _decisionsInProgress;

};

#endif // GAMEHISTORY_H
