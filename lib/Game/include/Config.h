#include <string>
#include <map>
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

    //Config();
    std::string getName();
    bool getAudience();

};

enum Kind{
    boolean = 0,
    integer = 1,
    string = 2,
    enumerator = 3,
    question_answer = 4,
};

struct Range{
    int start;
    int end;
};