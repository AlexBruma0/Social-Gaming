#include <string>
#include <map>
#include "Support.h"

using std::map;

class Config{
private:

    struct Setup{
        Kind kind;
        std::string prompt;
        Range range;
        int deflt;
    };

    std::string name;
    bool audience;

public:
    Range player_range;
    Setup setup;

    Config();
    std::string getName();
    bool getAudience();

};
