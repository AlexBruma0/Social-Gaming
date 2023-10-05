#include "Config.h"
class Game{
    public:
    Config config;
    map<std::string,map<std::string,std::string>> constants;
    map<std::string,std::string> variables;
    map<std::string,int> per_player;
    map<std::string,int> per_audience;
    //Structure of Rules TBD
    Game();
};