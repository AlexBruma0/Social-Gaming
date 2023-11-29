#include "GameState.h"

#include <nlohmann/json.hpp>
#include <variant>

using json = nlohmann::json;

GameVariables* GameState::getVars() {
    return &this->variables;
}

void GameState::setState(json* gameState){
    this->gameState = gameState;
}

void GameState::setVars(GameVariables gv) {
    this->variables = gv;
}

GameState::GameState(json* gameState, GameServer* server){
    this->gameState = gameState;
    this->server = server;
}

GameState::GameState() {
    this->gameState = nullptr;
}

json* GameState::getState() const{
    return this->gameState;
}

GameServer* GameState::getServer(){
    return server;
}

