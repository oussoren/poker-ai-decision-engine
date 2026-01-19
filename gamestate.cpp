#include "gamestate.h"
#include "player.h"

using namespace std;

Gamestate::Gamestate() {

}

Gamestate::Gamestate(const std::vector<std::shared_ptr<Player>>& players)
    : _players(players),
    currentPhase(GamePhase::preflop),
    _currentBet(0),
    _roundBet(0),
    _currentPlayerIndex(0),
    _dealerPosition(0),
    _smallBlindPosition(0),
    _bigBlindPosition(0),
    _smallBlind(0),
    _bigBlind(0),
    _bettingRound(1) {
    _communityCards.clear();
}

Gamestate::~Gamestate(){

}

double Gamestate::getPotOdds() const {
    if (_currentBet == 0) return 0.0;


    if (_currentPlayerIndex >= _players.size()) return 0.0;

    auto currentPlayer = _players[_currentPlayerIndex];
    int playerCurrentBet = currentPlayer->getRoundBet();
    int callAmount = _currentBet - playerCurrentBet;

    if (callAmount <= 0) return 0.0;

    return static_cast<double>(callAmount) / (callAmount + getTotalPotValue());
}

std::vector<std::shared_ptr<Player>> Gamestate::getActivePlayers(){
    vector<std::shared_ptr<Player>> active;
    for (auto& player: _players) {
        if (player->getAction() == 2) {
            active.push_back(player);
        }
    }
    return active;
}

std::vector<std::shared_ptr<Player>> Gamestate::getPlayers() const{
    return _players;
}

const std::vector<Card>& Gamestate::getCommunityCards() const {
    return _communityCards;
}

void Gamestate::addCommunityCards(Card card) {
    _communityCards.push_back(card);
    return;
}

GamePhase Gamestate::getCurrentPhase() const{
    return currentPhase;
}

int Gamestate::getCurrentPlayerIndex() const{
    return _currentPlayerIndex;
}

int Gamestate::getBettingRound() const{
    return _bettingRound;
}

// int Gamestate::getMainPot() const {
//     return _mainPot;
// }

// void Gamestate::addMainPot(int amount) {
//     _mainPot += amount;
//     _players[_currentPlayerIndexd] -= amount;
//     return;
// }

// int Gamestate::getSidePot() const {
//     return _sidePot;
// }

void Gamestate::setCurrentBet(int amount) {
    _currentBet = amount;
    return;
}

int Gamestate::getCurrentBet() const {
    return _currentBet;
}

int Gamestate::getRoundBet()  const{
    return _roundBet;
}

bool Gamestate::isPlayerActive(int index){
    return _players[index]->getAction() == 2;
}

void Gamestate::setBettingRound(int round) {
    _bettingRound = round;
    return;
}

void Gamestate::incrementBettingRound() {
    _bettingRound++;
    return;
}

void Gamestate::setDealerPosition(int position){
    _dealerPosition = position;
    return;
}

void Gamestate::setSmallBlindPosition(int position){
    _smallBlindPosition = position;
    return;
}

void Gamestate::setBigBlindPosition(int position){
    _bigBlindPosition = position;
    return;
}
void Gamestate::setCurrentPlayerIndex(int index){
    _currentPlayerIndex = index;
}

void Gamestate::rotateDealerPosition(){
    _dealerPosition = (_dealerPosition + 1) % _players.size();
}

void Gamestate::incrementCurrentPlayer(){
    _currentPlayerIndex = (_currentPlayerIndex + 1) % _players.size();
}


int Gamestate::getDealerPosition() const{
    return _dealerPosition;
}

int Gamestate::getSmallBlindPosition() const{
    return _smallBlindPosition;
}

int Gamestate::getBigBlindPosition() const{
    return _bigBlindPosition;
}

void Gamestate::reset() {

    currentPhase = GamePhase::preflop;
    _pots.clear();
    _currentBet = 0;
    _currentPlayerIndex = 0;
    _bettingRound = 1;
    _communityCards.clear();
    // _actionHistory.clear();
    return;
}

void Gamestate::setCurrentPhase(GamePhase phase) {
    currentPhase = phase;
    return;
}

void Gamestate::addPot(const Pot& pot) {
    _pots.push_back(pot);
}
const std::vector<Pot>& Gamestate::getPots() const {
    return _pots;
}
void Gamestate::clearPots() {
    _pots.clear();
    return;
}
int Gamestate::getTotalPotValue() const {
    int sum = 0;
    for (Pot pot: _pots) {
        sum += pot.amount;
    }
    return sum;
}

