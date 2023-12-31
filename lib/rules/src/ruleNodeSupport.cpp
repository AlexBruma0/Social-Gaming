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

std::unique_ptr<TreeNodeImpl> processFor(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ForNodeImpl>(getFirstLine(input), gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["var"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["collection"] = getNodeStringValue(tsNode.getChild(3), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("var", std::string(getNodeStringValue(tsNode.getChild(1), sourceCode)));
        vars.insert("collection", std::string(getNodeStringValue(tsNode.getChild(3), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processDiscard(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<DiscardNodeImpl>(input, gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["operand"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["collection"] = getNodeStringValue(tsNode.getChild(3), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("operand", std::string(getNodeStringValue(tsNode.getChild(1), sourceCode)));
        vars.insert("collection", std::string(getNodeStringValue(tsNode.getChild(3), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processMessage(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<MessageNodeImpl>(input, gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["target"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["message"] = getNodeStringValue(tsNode.getChild(2), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("target", std::string(getNodeStringValue(tsNode.getChild(1), sourceCode)));
        vars.insert("message", std::string(getNodeStringValue(tsNode.getChild(2), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processParallelFor(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ParallelForNodeImpl>(getFirstLine(input), gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["var"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["collection"] = getNodeStringValue(tsNode.getChild(3), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("var", std::string(getNodeStringValue(tsNode.getChild(1), sourceCode)));
        vars.insert("collection", std::string(getNodeStringValue(tsNode.getChild(3), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processInputChoice(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<InputChoiceNodeImpl>(input, gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["player"] = getNodeStringValue(tsNode.getChild(2), sourceCode);
        data["prompt"] = getNodeStringValue(tsNode.getChild(5), sourceCode);
        data["choices"] = getNodeStringValue(tsNode.getChild(7), sourceCode);
        data["target"] = getNodeStringValue(tsNode.getChild(9), sourceCode);
        data["timeout"] = getNodeStringValue(tsNode.getChild(11), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("player", std::string(getNodeStringValue(tsNode.getChild(2), sourceCode)));
        vars.insert("prompt", std::string(getNodeStringValue(tsNode.getChild(5), sourceCode)));
        vars.insert("choices", std::string(getNodeStringValue(tsNode.getChild(7), sourceCode)));
        vars.insert("target", std::string(getNodeStringValue(tsNode.getChild(9), sourceCode)));
        vars.insert("timeout", std::string(getNodeStringValue(tsNode.getChild(11), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processMatch(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<MatchNodeImpl>(input, gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["condition"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("condition", std::string(getNodeStringValue(tsNode.getChild(1), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processMatchEntry(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<MatchEntryNodeImpl>(getFirstLine(input), gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["condition"] = getNodeStringValue(tsNode.getChild(0), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("condition", std::string(getNodeStringValue(tsNode.getChild(0), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}


std::unique_ptr<TreeNodeImpl> processScores(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ScoresNodeImpl>(getFirstLine(input), gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["keys"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("keys", std::string(getNodeStringValue(tsNode.getChild(1), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processExtend(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<ExtendNodeImpl>(input, gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["target"] = getNodeStringValue(tsNode.getChild(1), sourceCode);
        data["with"] = getNodeStringValue(tsNode.getChild(3), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("target", std::string(getNodeStringValue(tsNode.getChild(1), sourceCode)));
        vars.insert("with", std::string(getNodeStringValue(tsNode.getChild(3), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}

std::unique_ptr<TreeNodeImpl> processAssignment(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, SendMessageQueue* in, ReceiveMessageQueue* out) {
    if (tsNode.getNumChildren() > 0) {
        std::string input = getNodeStringValue(tsNode, sourceCode);
        std::unique_ptr<TreeNodeImpl> implPtr = std::make_unique<AssignmentNodeImpl>(getFirstLine(input), gameState, in, out);

        json data = implPtr->getIdentifierData();
        data["target"] = getNodeStringValue(tsNode.getChild(0), sourceCode);
        data["value"] = getNodeStringValue(tsNode.getChild(2), sourceCode);
        implPtr->setIdentifierData(data);

        // new data setting; delete the one above when transition is complete
        GameVariables vars = implPtr->getNodeVariables();
        vars.insert("target", std::string(getNodeStringValue(tsNode.getChild(0), sourceCode)));
        vars.insert("value", std::string(getNodeStringValue(tsNode.getChild(2), sourceCode)));
        implPtr->setNodeVariables(vars);

        return implPtr;
    } else {
        return std::make_unique<TreeNodeImpl>("", gameState, in, out);
    }
}


void visitParallelInput(ParallelForNodeImpl* parent, const size_t& size, const std::string& responseName){
    // Wait for all responses from the input node child
    // TODO the timeout
    std::vector<int> responses;
    int tracking = 0;
    while(tracking < size){
        auto message = parent->getMessage();
        if(message.isValid()){
            responses.push_back(message.choice);
            std::string id= std::to_string(message.connection.id);

            // Reflects a response
            parent->enqueueMessage( id + " chose " + std::to_string(message.choice));
        }
        
        tracking++;
        
        
    }
    parent->getGameStateData()->getVars()->insert(responseName+"Responses", responses);

    // //Print Validation
    // auto choicesID = std::get<std::vector<int>>(parent->getGameStateData()->getVars()->getNestedMap(responseName+"Responses"));
    // for(auto c:choicesID){
    //     std::cout<< "Response " <<c<<std::endl;
    // }
}