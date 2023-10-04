#include "Config.h"
using std::string;
class Game{
    public:
    Config config;
    map<string,map<string,string>> constants;
    map<string,string> variables;
    map<string,int> per_player;
    map<string,int> per_audience;
    //Structure of Rules TBD
    Game();
};