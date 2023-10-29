/*
 Created by kevin on 10/11/2023.

 Edited:
    cstong (Caleb) 10/12/2023
*/

#include "parser.h"
#include "ruleNode.h"
#include "ruleNodeSupport.h"
#include <nlohmann/json.hpp>



/*
-------------------------------------------
TREE NODE CLASS 
-------------------------------------------
*/

TreeNode::TreeNode(std::string node, std::string type, GameState& gameState) : nodeType(type) {
    impl = std::move(this->parseNode(node, gameState));
    //std::cout << impl->getIdentifierData().dump();
    //std::cout << "address of impl " << impl.get() << std::endl;
}

TreeNode::TreeNode(TreeNode&& other) noexcept
    // transfer ownership to allow copying
    : impl(std::move(other.impl)) {
}

void TreeNode::addChild(std::unique_ptr<TreeNode> child) const { 
    impl->addChild(std::move(child)); 
}

void TreeNode::printTree(int depth) const {
    impl->printTree(depth);
}

std::unique_ptr<TreeNodeImpl> TreeNode::parseNode(const std::string& node, GameState& gameState){
    std::unordered_map<std::string, std::function<std::unique_ptr<TreeNodeImpl>(const std::string&, GameState&)>> typeToFunction;
    typeToFunction["for"] = processFor;
    typeToFunction["discard"] = processDiscard;
    typeToFunction["message"] = processMessage;
    typeToFunction["parallel_for"] = processParallelFor;
    typeToFunction["input_choice"] = processInputChoice;
//    typeToFunction["match"] = processMatch;
//    typeToFunction["scores"] = processScores;
//    typeToFunction["extend"] = processExtend;

    if (typeToFunction.count(nodeType) > 0) {
        std::unique_ptr<TreeNodeImpl> impl = typeToFunction[nodeType](node, gameState);
        //std::cout << std::endl << impl->getIdentifierData().dump();
        return std::move(impl);
    }
    return std::make_unique<TreeNodeImpl>("bad", gameState);
}

void TreeNode::execute() const{
    //std::cout<< "executing" <<std::endl;
    impl->execute();
}

/*
-------------------------------------------
NODE CLASSES
-------------------------------------------
*/


TreeNodeImpl::TreeNodeImpl(std::string id, GameState& _gameState): content(id), gameState(_gameState) {
    identifiers = json::parse("{}");
}

TreeNodeImpl::TreeNodeImpl() : content("") {
    identifiers = json::parse("{}");
    gameState = GameState();
}

TreeNodeImpl::~TreeNodeImpl(){}

void TreeNodeImpl::addChild(std::unique_ptr<TreeNode> child){
    children.push_back(std::move(child)); 
}

void TreeNodeImpl::printTree(int depth) const{
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    std::cout << content << std::endl;

    for (const auto& child : children) {
        child->printTree(depth + 1);
    }
}

void TreeNodeImpl::setIdentifierData(const json& data) {
    identifiers = data;
}

json TreeNodeImpl::getIdentifierData() const {
    return identifiers;
}

void TreeNodeImpl::execute(){
    //std::cout << children.size() << "\n";
    for (const auto& child : children) {
        child->execute();
    }
}

ForNodeImpl::ForNodeImpl(std::string id, GameState& _gameState): TreeNodeImpl(id, _gameState) {
    identifiers = json::parse("{}");
}

// Same as RuleNode Temp for testing
void ForNodeImpl::execute(){
    std::cout<< "executing for" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }
}

DiscardNodeImpl::DiscardNodeImpl(std::string id, GameState& _gameState): content(id), gameState(_gameState) {
    identifiers = json::parse("{}");
}

void DiscardNodeImpl::execute(){
    std::cout<< "executing discard" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }
}

MessageNodeImpl::MessageNodeImpl(std::string id, GameState& _gameState): content(id), gameState(_gameState) {
    identifiers = json::parse("{}");
}

void MessageNodeImpl::execute(){
    std::cout<< "executing message" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }
}

ParallelForNodeImpl::ParallelForNodeImpl(std::string id, GameState& _gameState): content(id), gameState(_gameState) {
    identifiers = json::parse("{}");
}

void ParallelForNodeImpl::execute(){
    std::cout<< "executing parallel for" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }
}

InputChoiceNodeImpl::InputChoiceNodeImpl(std::string id, GameState& _gameState): content(id), gameState(_gameState) {
    identifiers = json::parse("{}");
}

void InputChoiceNodeImpl::execute(){
    std::cout<< "executing input choice" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }
}