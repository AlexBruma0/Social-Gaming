
#include "parser.h"
#include <cstdio>
std::string file_to_string(std::string path) {
    FILE* f = fopen(path.c_str(), "r");
    if(!f){
        printf("Could not find file\n");
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);

    char game_string[size];
    rewind(f);
    fread(game_string, sizeof(char), size, f);

    return game_string;

}