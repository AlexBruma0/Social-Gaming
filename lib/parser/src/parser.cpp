
#include "parser.h"
#include <cstdio>
std::string file_to_string(const std::string path) {
    FILE* f = fopen(path.c_str(), "r");
    if(!f){
        printf("Could not find file\n");
        return std::string();
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);

    char game_string[size];
    rewind(f);
    fread(game_string, sizeof(char), size, f);

    fclose(f);
    return game_string;

}