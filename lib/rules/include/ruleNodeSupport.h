//
// Created by kevin on 10/11/2023.
//

#ifndef SOCIAL_GAMING_RULENODESUPPORT_H
#define SOCIAL_GAMING_RULENODESUPPORT_H

#include <string>
#include <vector>

std::vector<std::string> splitStringBySpace(const std::string& input);
std::string getFirstLine(const std::string& input);

std::unique_ptr<TreeNodeImpl> processFor(const std::string& op_string);
std::unique_ptr<TreeNodeImpl> processDiscard(const std::string op_string);
std::string processMessage(const std::string op_string);
std::string processParallelFor(const std::string op_string);
std::string processInputChoice(const std::string op_string);
std::string processMatch(const std::string op_string);
std::string processScores(const std::string op_string);
std::string processExtend(const std::string op_string);


#endif //SOCIAL_GAMING_RULENODESUPPORT_H
