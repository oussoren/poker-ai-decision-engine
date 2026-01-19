

#include "console.h"
#include "simpio.h"
#include "gamemanager.h"
#include "randombot.h"
#include <iostream>

int main() {
    // Create game with default rules

    std::cout << "Starting poker game with debug output...\n\n";

    GameManager game;

    // Add some players
    auto bot1 = std::make_shared<RandomBot>("Bot1", 1000, 0);
    auto bot2 = std::make_shared<RandomBot>("Bot2", 1000, 1);

    game.addPlayer(bot1);
    game.addPlayer(bot2);

    // Play a few hands
    std::cout << "Starting poker game...\n";
    game.playGame(5);  // Play 5 hands
    std::cout << "Game finished!\n";

    std::cout << "\nGame finished!\n";
    return 0;
}
