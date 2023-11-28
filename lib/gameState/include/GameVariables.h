
#ifndef KLAH_GAMEVARIABLES_H
#define KLAH_GAMEVARIABLES_H

#include <nlohmann/json.hpp>
#include <variant>

class GameVariables;


using json = nlohmann::json;

using ArrayType = std::variant<int, std::string, GameVariables>;
struct NotFoundTag {};

using GameValue = std::variant<int, std::string, std::vector<int>, std::vector<std::string>,
        std::vector<GameVariables>, std::vector<ArrayType>, GameVariables, ArrayType, NotFoundTag>;

class GameVariables {
public:
    void insert(const std::string& key, const GameValue& value);
    void erase(const std::string& key);
    GameValue getValue(const std::string& key);
    GameValue getNestedMap(const std::string& id);

    void print() const;
private:
    std::map<const std::string, GameValue> map;
};


#endif //KLAH_GAMEVARIABLES_H
