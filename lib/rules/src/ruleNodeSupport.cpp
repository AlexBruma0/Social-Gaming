#include <vector>
#include <string>
#include <sstream>
#include "ruleNode.h"
#include "cpp-tree-sitter.h"
#include <iostream>
#include "utils.h"

// doesn't work cause it thinks we're on C++11 somehow, and we can't use std::quoted
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

// split a string into tokens
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

// gets the content before a linebreak of a string
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

std::unique_ptr<TreeNodeImpl> processFor(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ForNodeImpl>(getFirstLine(input), gameState);

        json data = implPtr->getIdentifierData();
        data["var"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["collection"] = getNodeStringValue(tsNode.getChild(3), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processDiscard(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<DiscardNodeImpl>(input, gameState);

        json data = implPtr->getIdentifierData();
        data["operand"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["collection"] = getNodeStringValue(tsNode.getChild(3), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processMessage(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<MessageNodeImpl>(input, gameState);

        json data = implPtr->getIdentifierData();
        data["target"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["message"] = getNodeStringValue(tsNode.getChild(2), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processParallelFor(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ParallelForNodeImpl>(getFirstLine(input), gameState);

        json data = implPtr->getIdentifierData();
        data["var"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["collection"] = getNodeStringValue(tsNode.getChild(3), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processInputChoice(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<InputChoiceNodeImpl>(input, gameState);

        json data = implPtr->getIdentifierData();
        data["player"] = getNodeStringValue(tsNode.getChild(2), sourceCode);
        data["prompt"] = getNodeStringValue(tsNode.getChild(5), sourceCode);
        data["choices"] = getNodeStringValue(tsNode.getChild(7), sourceCode);
        data["target"] = getNodeStringValue(tsNode.getChild(9), sourceCode);
        data["timeout"] = getNodeStringValue(tsNode.getChild(11), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processMatch(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<MatchNodeImpl>(input, gameState);

        json data = implPtr->getIdentifierData();
        data["condition"] = getNodeStringValue(tsNode.getChild(1), sourceCode);

        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processMatchEntry(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<MatchEntryNodeImpl>(getFirstLine(input), gameState);

        json data = implPtr->getIdentifierData();
        data["condition"] = getNodeStringValue(tsNode.getChild(0), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}


std::unique_ptr<TreeNodeImpl> processScores(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ScoresNodeImpl>(getFirstLine(input), gameState);

        json data = implPtr->getIdentifierData();
        data["keys"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processExtend(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ExtendNodeImpl>(input, gameState);

        json data = implPtr->getIdentifierData();
        data["target"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["with"] = getNodeStringValue(tsNode.getChild(3), sourceCode);

        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}

std::unique_ptr<TreeNodeImpl> processAssignment(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<AssignmentNodeImpl>(getFirstLine(input), gameState);

        json data = implPtr->getIdentifierData();
        data["target"] = getNodeStringValue(tsNode.getChild(0), sourceCode);
        data["value"] = getNodeStringValue(tsNode.getChild(2), sourceCode);
        implPtr->setIdentifierData(data);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState);
    }
}
