#include "Game.h"

Game::Game(std::string gameString){
    this->gameString = gameString;
}

std::string Game::getString() const{
    return this->gameString;
}

void Game::setString(std::string s){
    this->gameString = s;
}

