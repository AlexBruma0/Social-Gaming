#include "include/Game.h"

Game::Game(Config config,
           map<string, map<string, string>> constants,
           map<string, string> variables,
           map<string, int> per_player,
           map<string, int> per_audience): 
           config(config), constants(constants), variables(variables),
           per_player(per_player), per_audience(per_audience){
}

