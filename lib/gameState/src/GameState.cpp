#include "GameState.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

GameState::GameState(json gameState){
    this->gameState = gameState;
}

GameState::GameState() {
    this->gameState = json::parse("{}");
}

json GameState::getState() const{
    return this->gameState;
}

void GameState::setState(json gameState){
    this->gameState = gameState;
}


