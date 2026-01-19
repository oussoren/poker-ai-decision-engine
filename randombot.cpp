#include "randombot.h"
#include "gamemanager.h"

RandomBot::RandomBot(const std::string& name, int chips, int position)
    : Player(name, chips, position) {
}

PlayerAction RandomBot::makeDecision(const Gamestate& gameState, GameManager* gameManager) {
    std::cout << "  " << getName() << " is making a decision...\n";

    int myIndex = -1;
    auto players = gameManager->getPlayers();

    std::cout << "    Number of players in gameState: " << players.size() << "\n";
    std::cout << "    My address: " << this << "\n";

    for (int i = 0; i < players.size(); i++) {
        std::cout << "    Player " << i << " address: " << players[i].get()
        << " name: " << players[i]->getName() << "\n";

        if (players[i].get() == this) {
            myIndex = i;
            std::cout << "    MATCH FOUND at index " << i << "\n";
            break;
        }
    }

    std::cout << "    My index: " << myIndex << "\n";

    if (myIndex != -1) {
        auto legalActions = gameManager->getLegalActions(myIndex);

        std::cout << "    Legal actions: " << legalActions.size() << "\n";

        for (auto& action : legalActions) {
            std::cout << "      - " << gameManager->actionToString(action) << "\n";
        }

        if (!legalActions.empty()) {
            int randomIndex = rand() % legalActions.size();
            std::cout << "    Choosing: " << gameManager->actionToString(legalActions[randomIndex]) << "\n";
            return legalActions[randomIndex];
        }
    }

    return PlayerAction(Action::fold);
}
