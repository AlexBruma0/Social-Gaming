#ifndef SOCIAL_GAMING_RULENODESUPPORT_H
#define SOCIAL_GAMING_RULENODESUPPORT_H

#include <string>
#include <vector>
#include "GameState.h"
#include "cpp-tree-sitter.h"

std::vector<std::string> splitStringBySpace(const std::string& input);
std::string getFirstLine(const std::string& input);

std::unique_ptr<TreeNodeImpl> processFor(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processDiscard(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processMessage(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processParallelFor(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processInputChoice(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processScores(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processAssignment(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processMatch(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processMatchEntry(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);
std::unique_ptr<TreeNodeImpl> processExtend(const ts::Node& tsNode, GameState* gameState, const std::string& sourceCode, const SendMessageQueue* in, const ReceiveMessageQueue* out);

GameValue parseResponse();
void visitParallelInput(ParallelForNodeImpl* parent, const size_t& size, const std::string& responseName);


#endif //SOCIAL_GAMING_RULENODESUPPORT_H
