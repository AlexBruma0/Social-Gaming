#ifndef SOCIAL_GAMING_RULENODESUPPORT_H
#define SOCIAL_GAMING_RULENODESUPPORT_H

#include <string>
#include <vector>

std::vector<std::string> splitStringBySpace(const std::string& input);
std::string getFirstLine(const std::string& input);

std::unique_ptr<TreeNodeImpl> processFor(const ts::Node& tsNode, GameState& gameState, const std::string& sourceCode);
// std::unique_ptr<TreeNodeImpl> processDiscard(const std::string op_string, GameState& gameState);
// std::unique_ptr<TreeNodeImpl> processMessage(const std::string op_string, GameState& gameState);
// std::unique_ptr<TreeNodeImpl> processParallelFor(const std::string op_string, GameState& gameState);
// std::unique_ptr<TreeNodeImpl> processInputChoice(const std::string op_string, GameState& gameState);
// std::unique_ptr<TreeNodeImpl> processMatch(const std::string op_string, GameState& gameState);
// std::unique_ptr<TreeNodeImpl> processScores(const std::string op_string, GameState& gameState);
// std::unique_ptr<TreeNodeImpl> processExtend(const std::string op_string, GameState& gameState);


#endif //SOCIAL_GAMING_RULENODESUPPORT_H
