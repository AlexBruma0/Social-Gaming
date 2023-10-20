#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameState{
    private:
    json gameState;
    public:
    json getState() const;
    void setState(json gameState);
    GameState(json gameState);
};