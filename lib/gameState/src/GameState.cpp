#include "GameState.h"

#include <nlohmann/json.hpp>
#include <variant>
#include <iostream>

using json = nlohmann::json;

    using ArrayType = std::variant<int, std::string, GameVariables>;

    using GameValue = std::variant<int, std::string, std::vector<int>, std::vector<std::string>, 
        std::vector<GameVariables>, std::vector<ArrayType>, GameVariables>;

GameState::GameState(json* gameState){
    this->gameState = gameState;
}

GameState::GameState() {
    this->gameState = nullptr;
}

json* GameState::getState() const{
    return this->gameState;
}

GameVariables* GameState::getVars() const{
    return this->variables;
}

void GameState::setState(json* gameState){
    this->gameState = gameState;
}

GameValue GameVariables::getValue(const std::string &key) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    } else {
        // return an empty value if nothing is found
        return {};
    }
}

void GameVariables::insert(const std::string &key, const GameValue &value) {
    map[key] = value;
}

void GameVariables::print() const {
    std::cout << "{";
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (it != map.begin()) {
            std::cout << ", ";
        }

        std::cout << "Key: \"" << it->first << "\", ";

        // Use if constexpr and type traits to check the type
        std::visit([](const auto& v) {
            if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int>) {
                std::cout << "Type: int, Value: \"" << v << "\"";
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                std::cout << "Type: std::string, Value: \"" << v << "\"";
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::vector<int>>) {
                std::cout << "Type: std::vector<int>, Value: [";
                for (auto vecIt = v.begin(); vecIt != v.end(); ++vecIt) {
                    std::cout << "\"" << *vecIt << "\"";
                    if (std::next(vecIt) != v.end()) {
                        std::cout << ", ";
                    }
                }
                std::cout << "]";
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::vector<std::string>>) {
                std::cout << "Type: std::vector<std::string>, Value: [";
                for (auto vecIt = v.begin(); vecIt != v.end(); ++vecIt) {
                    std::cout << "\"" << *vecIt << "\"";
                    if (std::next(vecIt) != v.end()) {
                        std::cout << ", ";
                    }
                }
                std::cout << "]";
            }
        }, it->second);
    }
    std::cout << "}" << std::endl;
}

std::pair<std::vector<std::string>, std::vector<std::string>> splitString(const std::string& s, const std::string delimiters) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = s.find_first_of(delimiters);

    std::vector<std::string> delimiterOrder;

    while (end != std::string::npos) {
        auto temp =s.substr(start, end - start);

        //Get the Delimiter here
        auto d =s.substr(end, 1);
        delimiterOrder.push_back(d);

        // Trim strings
        temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
        result.push_back(temp);
        start = end + 1;
        end = s.find_first_of(delimiters, start);
    }
    result.push_back(s.substr(start));

    return {result, delimiterOrder};
}

GameValue GameVariables::getNestedMap(const std::string& id){
    auto tokens= splitString(id, ".").first;
    GameValue gameData = getValue(tokens[0]);
    for (int i = 0; i < tokens.size(); i++) {
        if (std::holds_alternative<GameVariables>(gameData)) {
            gameData = std::get<GameVariables>(gameData).getValue(tokens[i]);
        } else {
            return gameData;
        }
    }

    return gameData;
}