#include "include/Game.h"

Game::Game(){
    this->config = new Config();
    this->constants = new map<string, map<string, string>>();
    this->variables = new map<string, string>();
    this->per_player = new map<string, int>();
    this->per_audience = new map<string, int>();
}

Game::~Game(){
    delete this->config;
    delete this->constants;
    delete this->per_audience;
    delete this->per_player;
    delete this->variables;
}

