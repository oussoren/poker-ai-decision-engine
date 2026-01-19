#include "player.h"

using namespace std;

// handle side pot implementation by making sure to keep track of each players side pots

/*
 *  std::string _name;
    int _position;
    int _chips;
    int _roundBet;
    int _totalBet;
    int _action; //-1 = nothing, 0 = folded,  1 = all in, 2 = active, 3 = sitting out, 4 = check, 5 = bet
    Hand _cards;
 */

Player::Player(const std::string& name, int chips, int position)
    : _name(name), _chips(chips), _position(position), _roundBet(0), _totalBet(0), _action(2) {
}

Player::~Player(){

}

bool Player::canAct() const{
    return _action != 0 && _action != 1 && _action != 3;
}

bool Player::canCall(int amount){
    if (isFolded() || isAllIn()) return false;

    int callAmount = amount - _roundBet;
    return _chips >= callAmount && callAmount > 0;
}

bool Player::canRaise(int currentBet, int minRaise){
    if (isFolded() || isAllIn()) return false;

    int raiseAmount = (currentBet - _roundBet) + minRaise;
    return _chips >= raiseAmount;
}

bool Player::canCheck(int currentBet){
    if (isFolded() || isAllIn()) return false;

    return _roundBet == currentBet;
}

bool Player::isFolded(){
    return _action == 0;
}

bool Player::isAllIn(){
    return _action == 1;
}

int Player::evaluateHand() const {
    return _cards.getHandRank();
}

int Player::getMaxBet() const{
    return _chips;
}

void Player::fold(){
    _action = 0;
}

bool Player::hasActed() const {
    return _action != 2;  // Assuming 2 = "active/waiting to act"
}

void Player::setBet(int amount){
    if (hasEnoughChips(amount)) {
        _roundBet = amount;
        _totalBet += amount;
        deductChips(amount);
        _action = 5;
        return;
    }
}

void Player::addToBet(int amount){
    _roundBet += amount;
    _totalBet += amount;
    _action = 5;
    return;
}

void Player::clearBet(){
    _roundBet = 0;
    _totalBet = 0;
    return;
}

void Player::check(){
    _action = 4;
    return;
}

void Player::goAllIn(){
    _roundBet = _chips;
    _totalBet += _chips;
    _chips = 0;
    _action = 1;
    return;
}

void Player::dealtCards(const vector<Card>& cards){
    for (Card card: cards) {
        _cards.addCard(card);
    }
    return;
}

std::string Player::getName(){
    return _name;
}

int Player::getPosition(){
    return _position;
}

int Player::getChips(){
    return _chips;
}

const Hand& Player::getHand() const{
    return _cards;
}

void Player::clearHand(){
    _cards.clear();
    return;
}

void Player::dealtCard(const Card& card) {
    _cards.addCard(card);
}

int Player::getAction(){
    return _action;
}

int Player::getRoundBet(){
    return _roundBet;
}

void Player::setRoundBet(int amount){
    _roundBet = amount;
    return;
}

void Player::clearRoundBet(){
    _roundBet = 0;
    return;
}

int Player::getTotalBet(){
    return _totalBet;
}

void Player::addChips(int amount){
    _chips += amount;
    return;
}

bool Player::deductChips(int amount){
    if (_chips >= amount) {
        _chips -= amount;
        return true;
    }
    return false;
}

bool Player::hasEnoughChips(int amount){
    return _chips >= amount;
}

void Player::reset(){
    _cards.clear();
    _action = 2;
    _roundBet = 0;
    _totalBet = 0;
}                         // Clear folded, allIn, currentBet, hand

