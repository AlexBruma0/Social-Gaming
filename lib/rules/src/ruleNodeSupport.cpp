//
// Created by kevin on 10/11/2023.
//
#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> splitStringBySpace(const std::string& input) {
    std::vector<std::string> tokens;
    bool insideQuotes = false;
    std::string token;

    for (char c : input) {
        if (c == '"') {
            insideQuotes = !insideQuotes;
            token += c;
        } else if (c == ' ' && !insideQuotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

// doesn't work cause it thinks we're on C++11 somehow
// if we can fix the version problem please use this one, it's much better.
//std::vector<std::string> splitStringBySpace(const std::string& input) {
//    std::vector<std::string> result;
//    std::istringstream iss(input);
//    std::string token;
//
//    while (iss >> std::quoted(token)) {
//        result.push_back(token);
//    }
//
//    return result;
//}

// use something like this for for loops (untested, but it looks like it should work)
std::string getFirstLine(const std::string& input) {
    std::size_t newlinePos = input.find('\n');

    if (newlinePos != std::string::npos) {
        // found a newline, return the part before it
        return input.substr(0, newlinePos);
    } else {
        // no newline found, return the entire string
        return input;
    }
}
