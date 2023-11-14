#include <string>
#include <nlohmann/json.hpp>
#include <variant>
#pragma once

using json = nlohmann::json;

class GameVariables {
public:
    using ArrayType = std::variant<int, std::string, GameVariables>;

    using GameValue = std::variant<int, std::string, std::vector<int>, std::vector<std::string>, 
    std::vector<GameVariables>,std::vector<ArrayType>, GameVariables>;

    void insert(const std::string& key, const GameValue& value);
    GameValue getValue(const std::string& key);
    void print() const;
private:
    std::map<std::string, GameValue> map;
};

class GameState{
private:
    json* gameState;
    GameVariables* variables;
public:
    json* getState() const;
    void setState(json* gameState);
    GameVariables* getVars() const;
    GameState(json* gameState);
    GameState();
};

