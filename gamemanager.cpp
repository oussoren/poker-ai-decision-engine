#include "gamemanager.h"

// member Vars
// RuleSet _rules;
// Gamestate _current;
// std::vector<std::shared_ptr<Player>> _players;
// Deck _deck;
// int _smallBlindAmt;
// int _bigBlindAmt;
// int _maxRaises; //max raises per betting round based on the ruleset
// int _startingChips;
// int _handNumber;
// //record betting round or just record it as a game phase???
// bool _gameActive;
// bool _handInProgress;

GameManager::GameManager()
    : _rules(),  // Uses RuleSet default constructor
    _smallBlindAmt(_rules.getSmallBlind()),
    _bigBlindAmt(_rules.getBigBlind()),
    _maxRaises(_rules.getMaxRaises()),
    _startingChips(_rules.getStartingChips()),
    _handNumber(0),
    _gameActive(false),
    _anyPlayerActedThisRound(false),
    _handInProgress(false),
    _current() {
}

GameManager::GameManager(const RuleSet& rules)
    : _rules(rules),
    _smallBlindAmt(_rules.getSmallBlind()),
    _bigBlindAmt(_rules.getBigBlind()),
    _maxRaises(_rules.getMaxRaises()),
    _startingChips(_rules.getStartingChips()),
    _handNumber(0),
    _gameActive(false),
    _handInProgress(false),
    _anyPlayerActedThisRound(false),
    _current() {
}

GameManager::GameManager(int smallBlind, int bigBlind, int startingChips)
    : _rules(smallBlind, bigBlind, startingChips),  // Create RuleSet with these values
    _smallBlindAmt(_rules.getSmallBlind()),
    _bigBlindAmt(_rules.getBigBlind()),
    _maxRaises(_rules.getMaxRaises()),
    _startingChips(_rules.getStartingChips()),
    _handNumber(0),
    _gameActive(false),
    _handInProgress(false),
    _anyPlayerActedThisRound(false),
    _current() {
}

GameManager::~GameManager(){

}

std::vector<std::shared_ptr<Player>> GameManager::getPlayers() {
    return _players;
}

void GameManager::addPlayer(std::shared_ptr<Player> player){
    _players.push_back(player);
}

void GameManager::startNewHand(){
    std::cout << "\n=== STARTING HAND " << _handNumber + 1 << " ===\n";

    // Show player chip counts
    for (int i = 0; i < _players.size(); i++) {
        std::cout << _players[i]->getName() << ": $" << _players[i]->getChips() << " chips\n";
    }



    _deck.shuffle();
    _current.reset();

    for (auto& player : _players) {
        player->reset();
    }

    rotateDealer();
    updateBlindPositions();
    setCurrentPlayerToFirstActive();
    _handInProgress = true;
    _handNumber++;

    std::cout << "Dealer: Position " << _current.getDealerPosition()
              << ", SB: Position " << _current.getSmallBlindPosition()
              << ", BB: Position " << _current.getBigBlindPosition() << "\n";
}

void GameManager::dealHoleCards(){
    for (int numCards = 0; numCards < 2; numCards++) {
        for (auto& player : _players) {
            if (isPlayerActive(player->getPosition())) {
                player->dealtCard(_deck.deal());
            }
        }
    }
}

void GameManager::startNewBettingRound(){
    for (auto& player : _players) {
        player->clearRoundBet();
    }
    return;
}

void GameManager::runBettingRound() {
    _anyPlayerActedThisRound = false;


    while (!isBettingRoundComplete()) {
        // Player acts
        int currentPlayerIndex = _current.getCurrentPlayerIndex();
        std::shared_ptr<Player> player = _players[currentPlayerIndex];
        PlayerAction action = player->makeDecision(_current, this);
        handlePlayerAction(action, currentPlayerIndex);

        _anyPlayerActedThisRound = true;

        if (isHandComplete()) break;  // This might be triggering

        // Move to next player
        _current.setCurrentPlayerIndex(getNextActivePlayer(currentPlayerIndex));
    }

    std::cout << "Betting round loop completed, calling collectBets()\n";
    collectBets();
    std::cout << "collectBets() completed\n";
}

std::string GameManager::actionToString(const PlayerAction& action) {
    switch(action.actionType) {
    case Action::fold: return "FOLD";
    case Action::check: return "CHECK";
    case Action::call: return "CALL $" + std::to_string(action.amount);
    case Action::bet: return "BET $" + std::to_string(action.amount);
    case Action::raise: return "RAISE to $" + std::to_string(action.amount);
    case Action::all_in: return "ALL-IN $" + std::to_string(action.amount);
    default: return "UNKNOWN";
    }
}

void GameManager::handlePlayerAction(const PlayerAction& playerAct, int playerIndex){
    auto player = _players[playerIndex];

    // 1. Validate the action is legal
    if (!validateAction(playerAct, playerIndex)) {
        // For now, just return - in real game might ask again
        return;
    }

    // 2. Apply the action effects
    switch(playerAct.actionType) {
    case Action::fold:
        player->fold();
        break;

    case Action::check:
        player->check();
        break;

    case Action::call: {
        int currentBet = _current.getCurrentBet();
        int callAmount = currentBet - player->getRoundBet();
        player->addToBet(callAmount);
        break;
    }

    case Action::bet:
    case Action::raise: {

        int betAmount = playerAct.amount;
        int addAmount = betAmount - player->getRoundBet();
        player->addToBet(addAmount);
        _current.setCurrentBet(betAmount);
        break;
    }

    case Action::all_in:
        player->goAllIn();
        int totalBet = player->getRoundBet() + player->getChips();
        _current.setCurrentBet(std::max(_current.getCurrentBet(), totalBet));
        break;
    }

    // 3. Record action for history/ML (optional for now)
    // _current.addActionToHistory(action, playerIndex);

    // 4. Check for special conditions
    if (player->getChips() == 0 && playerAct.actionType != Action::all_in) {
        player->goAllIn();  // Went all-in accidentally
    }
}

std::shared_ptr<Player> GameManager::determineWinner(const std::vector<int>& elligiblePlayerIndices) {
    std::vector<std::pair<std::shared_ptr<Player>, Hand>> playerHands;

    // Collect hands from eligible players
    for (int playerIndex : elligiblePlayerIndices) {
        auto player = _players[playerIndex];
        if (!player->isFolded()) {
            playerHands.push_back({player, player->getHand()});
        }
    }

    if (playerHands.empty()) return nullptr;

    auto winner = std::max_element(playerHands.begin(), playerHands.end(),[](const auto& a, const auto& b) {
        return b.second > a.second;
    });

    return winner->first;
}

void GameManager::playHand() {
    startNewHand();
    dealHoleCards();
    collectBlinds();

    // Preflop betting
    runBettingRound();
    if (isHandComplete()) {
        endHand();
        return;
    }

    // Flop
    advancePhase();
    dealFlop();
    if (canMoreBettingOccur()) {
        runBettingRound();
        if (isHandComplete()) {
            endHand();
            return;
        }
    }

    // Turn
    advancePhase();
    dealTurn();
    if (canMoreBettingOccur()) {
        runBettingRound();
        if (isHandComplete()) {
            endHand();
            return;
        }
    }

    // River
    advancePhase();
    dealRiver();
    if (canMoreBettingOccur()) {
        runBettingRound();
    }

    // End hand (showdown or winner determination)
    endHand();
}

void GameManager::playGame(int numHands){
    _gameActive = true;

    for (int i = 0; i < numHands && !isGameOver(); i++) {
        playHand();
    }

    _gameActive = false;
}

void GameManager::advancePhase() {
    GamePhase currentPhase = _current.getCurrentPhase();

    switch(currentPhase) {
    case GamePhase::preflop:
        _current.setCurrentPhase(GamePhase::flop);
        _current.incrementBettingRound();
        break;
    case GamePhase::flop:
        _current.setCurrentPhase(GamePhase::turn);
        _current.incrementBettingRound();
        break;
    case GamePhase::turn:
        _current.setCurrentPhase(GamePhase::river);
        _current.incrementBettingRound();
        break;
    case GamePhase::river:
        _current.setCurrentPhase(GamePhase::showdown);
        _current.incrementBettingRound();
        break;
    }
    _anyPlayerActedThisRound = false;
    return;
}

const Gamestate& GameManager::getGameState() const{
    return _current;
}

void GameManager::removePlayer(int playerIndex){
    _players.erase(_players.begin() + playerIndex);
    return;
}

int GameManager::getNextActivePlayer(int currentPlayerIndex){
    if (_players.empty()) return -1;

    int nextIndex = currentPlayerIndex;
    int playersChecked = 0;

    nextIndex = (nextIndex + 1) % _players.size();
    playersChecked++;

    while (!isPlayerActive(nextIndex) && playersChecked < _players.size()) {
        nextIndex = (nextIndex + 1) % _players.size();
        playersChecked++;
    }

    if (playersChecked >= _players.size()) {
        return -1;
    }

    return nextIndex;

}

void GameManager::rotateDealer() {
    // Move dealer button to next active player
    int newDealer = getNextActivePlayer(_current.getDealerPosition());
    _current.setDealerPosition(newDealer);
}

void GameManager::setCurrentPlayerToFirstActive() {
    // Start with player after big blind for preflop
    int bbPos = _current.getBigBlindPosition();
    int firstPlayer = getNextActivePlayer(bbPos);
    _current.setCurrentPlayerIndex(firstPlayer);
}

void GameManager::updateBlindPositions() {
    int dealerPos = _current.getDealerPosition();
    int sbPos = getNextActivePlayer(dealerPos);
    int bbPos = getNextActivePlayer(sbPos);

    _current.setSmallBlindPosition(sbPos);
    _current.setBigBlindPosition(bbPos);
}

bool GameManager::isPlayerActive(int playerIndex){
    return _players[playerIndex]->canAct();
}

void GameManager::dealFlop(){
    Card burnCard = _deck.deal();
    _current.addCommunityCards(_deck.deal());
    _current.addCommunityCards(_deck.deal());
    _current.addCommunityCards(_deck.deal());
    return;
}

void GameManager::dealTurn(){
    Card burnCard = _deck.deal();
    _current.addCommunityCards(_deck.deal());
    return;
}

void GameManager::dealRiver(){
    Card burnCard = _deck.deal();
    _current.addCommunityCards(_deck.deal());
    return;
}

void GameManager::collectBlinds(){
    //small blind and big blind positions
    int sB = _current.getSmallBlindPosition();
    int bB = _current.getBigBlindPosition();

    //take from small blind and handle all in case
    auto sBPlayer = _players[sB];
    int sBAmount = std::min(_smallBlindAmt, sBPlayer->getChips());
    sBPlayer->setBet(sBAmount);

    if (sBAmount < _smallBlindAmt) {
        sBPlayer->goAllIn();
    }
    //take from big blind if possible
    auto bBPlayer = _players[bB];
    int bBAmount = std::min(_bigBlindAmt, bBPlayer->getChips());
    bBPlayer->setBet(bBAmount);

    if (bBAmount < _bigBlindAmt) {
        bBPlayer->goAllIn();
    }

    _current.setCurrentBet(std::max(sBAmount, bBAmount));
    return;
}

bool GameManager::validateAction(const PlayerAction& action, int playerIndex) {
    auto player = _players[playerIndex];
    int currentBet = _current.getCurrentBet();

    // Basic checks
    if (player->isFolded() || !player->canAct()) {
        return false;
    }

    switch(action.actionType) {
    case Action::fold:
    case Action::check:
    case Action::all_in:
        return true;  // Amount doesn't matter for these

    case Action::call: {
        int callAmount = currentBet - player->getRoundBet();
        return callAmount > 0 && player->hasEnoughChips(callAmount);
    }

    case Action::bet: {
        return currentBet == 0 &&
               action.amount >= _bigBlindAmt &&  // Check actual bet amount
               player->hasEnoughChips(action.amount);
    }

    case Action::raise: {
        int minRaise = getMinimumRaise();
        return currentBet > 0 &&
               action.amount >= minRaise &&     // Check actual raise amount
               player->hasEnoughChips(action.amount - player->getRoundBet());
    }
    }

    return false;
}

void GameManager::collectBets() {
    std::cout << "collectBets(): Starting\n";

    for (auto& player : getActivePlayers()) {
        std::cout << "Clearing round bet for " << player->getName() << "\n";
        player->clearRoundBet();
    }

    std::cout << "collectBets(): Calling calculatePots()\n";
    calculatePots();
    std::cout << "collectBets(): calculatePots() completed\n";

    _current.setCurrentBet(0);
    std::cout << "collectBets(): Completed\n";
}

void GameManager::calculatePots() {
    std::cout << "calculatePots(): Starting\n";
    _current.clearPots();

    std::vector<std::pair<int, int>> playerContributions;
    for (int i = 0; i < _players.size(); i++) {
        int totalBet = _players[i]->getTotalBet();
        if (totalBet > 0) {
            playerContributions.push_back({i, totalBet});
            std::cout << "Player " << i << " (" << _players[i]->getName()
                      << ") contributed: $" << totalBet << "\n";
        }
    }

    if (playerContributions.empty()) return;

    std::sort(playerContributions.begin(), playerContributions.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    int previousLevel = 0;

    for (int i = 0; i < playerContributions.size(); i++) {
        int currentLevel = playerContributions[i].second;
        int potContribution = currentLevel - previousLevel;

        if (potContribution > 0) {
            // FIXED: Count how many players are still in at this level
            int playersAtThisLevel = playerContributions.size();  // All players contribute to this level
            int potAmount = potContribution * playersAtThisLevel;

            std::cout << "Creating pot: contribution=" << potContribution
                      << " x " << playersAtThisLevel << " players = $" << potAmount << "\n";

            Pot newPot(potAmount);

            // Add eligible players (those who contributed at least this much and aren't folded)
            for (int j = i; j < playerContributions.size(); j++) {
                int playerIndex = playerContributions[j].first;
                if (!_players[playerIndex]->isFolded()) {
                    newPot.eligiblePlayerIndices.push_back(playerIndex);
                    std::cout << "  Eligible: " << _players[playerIndex]->getName() << "\n";
                }
            }

            if (!newPot.eligiblePlayerIndices.empty()) {
                _current.addPot(newPot);
            }
        }

        previousLevel = currentLevel;
    }

    std::cout << "calculatePots(): Completed\n";
}

bool GameManager::canMoreBettingOccur() {
    int playersWhoCanAct = 0;
    for (auto& player : _players) {
        if (player->canAct()) {  // Not folded, not all-in
            playersWhoCanAct++;
        }
    }
    return playersWhoCanAct >= 2;  // Need at least 2 players who can act
}

int GameManager::getMinimumBet() {
    return _rules.getMinimumRaise(_current.getCurrentBet());
}

int GameManager::getMinimumRaise() {
    return _rules.getMinimumRaise(_current.getCurrentBet());
}

int GameManager::getMaximumBet(int playerIndex) {
    auto player = _players[playerIndex];
    return _rules.getMaximumBet(_current.getCurrentBet(), player->getChips(), _current.getTotalPotValue());
}

bool GameManager::isBettingRoundComplete() {
    if (!_anyPlayerActedThisRound) {
        std::cout << "  No one has acted yet, continuing round\n";
        return false;
    }

    int currentBet = _current.getCurrentBet();
    int activePlayers = 0;
    int playersWhoActed = 0;

    std::cout << "  Checking if betting round complete:\n";

    for (auto& player : _players) {
        if (player->canAct()) {
            activePlayers++;
            std::cout << "    " << player->getName() << ": canAct=true, roundBet=" << player->getRoundBet()
                      << ", currentBet=" << currentBet << ", folded=" << player->isFolded()
                      << ", allIn=" << player->isAllIn() << "\n";

            if (player->getRoundBet() == currentBet || player->isFolded() || player->isAllIn()) {
                playersWhoActed++;
                std::cout << "      -> Counted as acted\n";
            } else {
                std::cout << "      -> NOT counted as acted\n";
            }
        } else {
            std::cout << "    " << player->getName() << ": canAct=false\n";
        }
    }

    // FIX: Change the condition
    bool complete = (playersWhoActed == activePlayers) && (activePlayers >= 0);  // Remove > 1 requirement
    std::cout << "  Result: " << playersWhoActed << "/" << activePlayers << " acted, complete=" << complete << "\n";

    return complete;
}

bool GameManager::isHandComplete() {
    auto activePlayers = getActivePlayers();
    int numActive = activePlayers.size();

    std::cout << "  Checking if hand complete: " << numActive << " active players\n";

    if (numActive <= 1) {
        std::cout << "    -> Hand complete: Only " << numActive << " active players\n";
        return true;
    }

    if (_current.getCurrentPhase() == GamePhase::river && isBettingRoundComplete()) {
        std::cout << "    -> Hand complete: River phase and betting complete\n";
        return true;
    }

    // Check if everyone is all-in
    for (auto& player: activePlayers) {
        if (!player->isAllIn()) {
            std::cout << "    -> Hand continues: " << player->getName() << " is not all-in\n";
            return false;
        }
    }

    std::cout << "    -> Hand complete: Everyone is all-in\n";
    return true;
}

bool GameManager::isGameOver() {
    // Game ends when only 1 player has chips
    int playersWithChips = 0;
    for (auto& player : _players) {
        if (player->getChips() > 0) {
            playersWithChips++;
        }
    }
    return playersWithChips <= 1;
}

void GameManager::endHand(){
    std::cout << "\n--- HAND COMPLETE ---\n";

    auto pots = _current.getPots();
    std::cout << "Number of pots: " << pots.size() << "\n";
    for (int i = 0; i < pots.size(); i++) {
        std::cout << "Pot " << i+1 << ": $" << pots[i].amount << " (";
        for (int playerIdx : pots[i].eligiblePlayerIndices) {
            std::cout << _players[playerIdx]->getName() << " ";
        }
        std::cout << ")\n";
    }

    distributeWinnings();

    std::cout << "\nFinal chip counts:\n";
    for (auto& player : _players) {
        std::cout << player->getName() << ": $" << player->getChips() << "\n";
    }

    // Update statistics
    // updatePlayerStats();

    // Collect training data (for ML)
    // if (dataCollector) {
    //     collectTrainingData();
    // }

    // Set flags
    _handInProgress = false;

    // Check if game should end
    removeEliminatedPlayers();  // Players with 0 chips -- need to implement
    if (isGameOver()) {
        std::cout << "\n*** GAME OVER ***\n";
        _gameActive = false;
    }
}

void GameManager::removeEliminatedPlayers() {
    for (int i = _players.size() - 1; i >= 0; i--) {
        if (_players[i]->getChips() <= 0) {
            _players.erase(_players.begin() + i);
        }
    }
}

std::vector<std::shared_ptr<Player>> GameManager::getActivePlayers() {
    std::vector<std::shared_ptr<Player>> activePlayers;
    for (auto& player : _players) {
        // Active = not folded (all-in players are still active for hand completion)
        if (!player->isFolded()) {
            activePlayers.push_back(player);
        }
    }
    return activePlayers;
}

void GameManager::distributeWinnings(){
    for (Pot pot: _current.getPots()) {
        if (pot.amount == 0) continue;

        auto winner = determineWinner(pot.eligiblePlayerIndices);

        if (winner) {
            winner->addChips(pot.amount);
        }
    }
}

// Change the return type
std::vector<PlayerAction> GameManager::getLegalActions(int playerIndex) {
    std::vector<PlayerAction> legalActions;
    auto player = _players[playerIndex];
    int currentBet = _current.getCurrentBet();

    // Actions that don't need amounts
    if (!player->isFolded()) {
        legalActions.push_back(PlayerAction(Action::fold));
    }

    if (currentBet == 0 || player->getRoundBet() == currentBet) {
        legalActions.push_back(PlayerAction(Action::check));
    }

    if (currentBet > player->getRoundBet() && player->canCall(currentBet)) {
        int callAmount = currentBet - player->getRoundBet();
        legalActions.push_back(PlayerAction(Action::call, callAmount));  // Add amount
    }

    // Actions that need amounts
    if (currentBet == 0 && player->getChips() > 0) {
        int minBet = _bigBlindAmt;
        legalActions.push_back(PlayerAction(Action::bet, minBet));
    }

    if (currentBet > 0 && player->canRaise(currentBet, getMinimumRaise())) {
        int minRaise = getMinimumRaise();
        legalActions.push_back(PlayerAction(Action::raise, minRaise));
    }

    if (player->getChips() > 0) {
        int allInAmount = player->getRoundBet() + player->getChips();
        legalActions.push_back(PlayerAction(Action::all_in, allInAmount));
    }

    return legalActions;
}
