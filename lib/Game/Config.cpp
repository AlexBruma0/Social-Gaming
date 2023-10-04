#include "include/Config.h"

std::string Config::getName(){
    return this->name;
}

bool Config::getAudience(){
    return this->audience;
}

Config::Config(){
    this->name = "";
    this->player_range = Range(0, 0);
    this->audience = false;
    this->setup.kind = undefined;
    this->setup.prompt = "undefined";
    this->setup.range = Range(0, 0);
}