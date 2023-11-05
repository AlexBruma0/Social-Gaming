#include <string>
#include <nlohmann/json.hpp>
#pragma once

using json = nlohmann::json;

class GameState{
    private:
    json* gameState;
    public:
    json* getState() const;
    void setState(json* gameState);
    GameState(json* gameState);
    GameState();
};