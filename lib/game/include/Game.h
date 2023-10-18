#include <string>

class Game{
    private:
    std::string gameString;
    public:
    std::string getString() const;
    void setString(std::string gameString);
    Game(std::string gameString);
};