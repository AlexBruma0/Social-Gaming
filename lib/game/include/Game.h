#include <string>
class Game{
    private:
    std::string gameString;
    public:
    Game(std::string gamestring);
    std::string getString() const;
    void setString(std::string s);
};