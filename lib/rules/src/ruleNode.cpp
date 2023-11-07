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

TreeNode::TreeNode(const ts::Node& tsNode, std::string type, const std::string& sourceCode , GameState* gameState) : nodeType(type) {
    impl = std::move(this->parseNode(tsNode, gameState, sourceCode));
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

void TreeNode::update(){
    impl->update();
}

std::unique_ptr<TreeNodeImpl> TreeNode::parseNode(const ts::Node tsNode, GameState* gameState,const std::string& source_code) {
    std::unordered_map<std::string, std::function<std::unique_ptr<TreeNodeImpl>(const ts::Node&, GameState*, const std::string& )>> typeToFunction;
    typeToFunction["for"] = processFor;
    typeToFunction["discard"] = processDiscard;
    typeToFunction["message"] = processMessage;
    typeToFunction["parallel_for"] = processParallelFor;
    typeToFunction["input_choice"] = processInputChoice;
    typeToFunction["match"] = processMatch;
    typeToFunction["match_entry"] = processMatchEntry;
    typeToFunction["extend"] = processExtend;
    typeToFunction["scores"] = processScores;
    typeToFunction["assignment"] = processAssignment;

    if (typeToFunction.count(nodeType) > 0) {
        std::unique_ptr<TreeNodeImpl> impl = typeToFunction[nodeType](tsNode, gameState, source_code);
        //std::cout << std::endl << impl->getIdentifierData().dump();
        return std::move(impl);
    }
    return std::make_unique<TreeNodeImpl>("", gameState);
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


TreeNodeImpl::TreeNodeImpl(std::string id, GameState* _gameState): content(id), gameState(_gameState) {
    identifiers = json::parse("{}");
}

TreeNodeImpl::TreeNodeImpl() : content("") {
    identifiers = json::parse("{}");
    gameState = nullptr;
}

TreeNodeImpl::~TreeNodeImpl(){}

void TreeNodeImpl::addChild(std::unique_ptr<TreeNode> child){
    children.push_back(std::move(child)); 
}

void TreeNodeImpl::printTree(int depth) const{
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }

    std::cout << "identifiers: " << identifiers << std::endl;

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

void TreeNodeImpl::update(){

}

void TreeNodeImpl::execute(){
    std::cout << children.size() << "\n";
    for (const auto& child : children) {
        child->execute();
    }
}

ForNodeImpl::ForNodeImpl(std::string id, GameState* _gameState): TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "for"})");
}

void ForNodeImpl::update(){
    std::cout << "updating" <<std::endl;
}

// Same as RuleNode Temp for testing
void ForNodeImpl::execute(){
    json gameJson = getIdentifierData();

    // Gets the first element
    // Assuming that it will be parsed and contain only one array
    // Temporary until the id are decided
    auto array = gameJson[TreeNodeImpl::COLLECTION_ID];
    
    //std::cout<< "executing for" <<std::endl;

    // For all the elements in the array execute the child code
    size_t index = 0;
    for (auto el: array) {
        for (const auto& child : children) {
            
            child->execute();
            child->update();
        }
        index ++;
    }
    
}

DiscardNodeImpl::DiscardNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "discard"})");
}

void DiscardNodeImpl::execute(){
    std::cout<< "executing discard" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: read the collection from the gamestate, then find the item and drop it
}

MessageNodeImpl::MessageNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "message"})");
}

void MessageNodeImpl::execute(){
    std::cout<< "executing message" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: once Arjun has the server I/O interface ready, call it here to send the message
}

ParallelForNodeImpl::ParallelForNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "parallel_for"})");
}

void ParallelForNodeImpl::execute(){
    std::cout<< "executing parallel for" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }
}

InputChoiceNodeImpl::InputChoiceNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "input_choice"})");
}

void InputChoiceNodeImpl::execute(){
    std::cout<< "executing input choice" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: once Arjun has the server I/O interface ready, call it here to send prompt and get answer (probably with IDs)
}

ScoresNodeImpl::ScoresNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "scores"})");
}

void ScoresNodeImpl::execute(){
    std::cout<< "executing scores" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: get scores from game state, sort it, then message it to everyone
}

AssignmentNodeImpl::AssignmentNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "assignment"})");
}

void AssignmentNodeImpl::execute(){
    std::cout<< "executing assignment" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: get the variable (or make one, if needed), write the value, then write it to the game state
}

MatchNodeImpl::MatchNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "match"})");
}

void MatchNodeImpl::execute(){
    std::cout<< "executing match" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: evaluate the condition and then write it to the game state, to be compared against the match_entry nodes
}

MatchEntryNodeImpl::MatchEntryNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "match_entry"})");
}

void MatchEntryNodeImpl::execute(){
    std::cout<< "executing match entry" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: evaluate the condition and then compare it to the game state's stored match expression
}

ExtendNodeImpl::ExtendNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "extend"})");
}

void ExtendNodeImpl::execute(){
    std::cout<< "executing extend" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: get the collection from the game state, append the list or element onto it, then write it again
}
