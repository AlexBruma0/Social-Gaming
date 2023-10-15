#include <string>

class Game{
    std::string gameString;
    std::string getString() const;
    void setString(std::string gameString);
    Game(std::string gameString);
};