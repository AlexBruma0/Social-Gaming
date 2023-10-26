//
// Created by kevin on 10/11/2023.
//
#include <vector>
#include <string>
#include <sstream>
#include "ruleNode.h"
#include <iostream>

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

std::unique_ptr<TreeNodeImpl> processFor(const std::string& op_string, GameState& gameState) {
    std::vector<std::string> tokens = splitStringBySpace(op_string);
    if (tokens.size() > 1) {
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ForNodeImpl>(getFirstLine(op_string), gameState);
        //orNodeImpl impl(getFirstLine(op_string), gameState);

        json data = json::parse("{}");
        data["var"] = tokens[1];
        data["collection"] = tokens[3];
        implPtr->setIdentifierData(data);

        std::cout << "\nfrom processFor data: " << data.dump();
        std::cout << "\nfrom processFor impl: " << implPtr->getIdentifierData().dump();

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

// std::unique_ptr<TreeNodeImpl> processDiscard(const std::string op_string, GameState& gameState) {
//     if (splitStringBySpace(op_string).size() == 4) {
//         return std::make_unique<DiscardNodeImpl>(op_string, gameState);
//     } else {
//         return std::make_unique<TreeNodeImpl>("", gameState);
//     }
// }

// std::string processMessage(const std::string op_string) {
//     return (splitStringBySpace(op_string).size() == 3) ? op_string : "";
// }

// std::string processParallelFor(const std::string op_string) {
//     return (splitStringBySpace(op_string).size() > 1) ? getFirstLine(op_string) : "";
// }

// std::string processInputChoice(const std::string op_string) {
//     return (splitStringBySpace(op_string).size() > 1) ? op_string : "";
// }

// std::string processMatch(const std::string op_string) {
//     return (splitStringBySpace(op_string).size() > 1) ? op_string : "";
// }

// std::string processScores(const std::string op_string) {
//     return (splitStringBySpace(op_string).size() == 2) ? op_string : "";
// }

// std::string processExtend(const std::string op_string) {
//     return (splitStringBySpace(op_string).size() > 3) ? op_string : "";
// }

