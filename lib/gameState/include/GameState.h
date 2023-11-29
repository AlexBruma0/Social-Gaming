#include <string>
#include <nlohmann/json.hpp>
#include <variant>
#include "gameServer.h"
#include "GameVariables.h"

#pragma once

class GameState{
private:
    json* gameState;
    GameServer* server;
    GameVariables variables;
public:
    json* getState() const;
    void setState(json* gameState);
    GameVariables* getVars();
    void setVars(GameVariables gv);
    GameServer* getServer();
    GameState(json* gameState, GameServer* server);
    GameState();
};

