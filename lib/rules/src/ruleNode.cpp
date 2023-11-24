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

std::string TreeNode::getType(){
    return nodeType;
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
    GameVariables gv;
    gv.insert("_type", std::string("generic"));
    nodeVariables = gv;
}

TreeNodeImpl::TreeNodeImpl() : content("") {
    identifiers = json::parse("{}");
    nodeVariables = GameVariables();
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

    //std::cout << "identifiers: " << identifiers << std::endl;
    std::cout << "node vars: ";
    nodeVariables.print();
    std::cout << std::endl;

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

GameState* TreeNodeImpl::getGameStateData(){
    return gameState;
}

void TreeNodeImpl::execute(){
    //std::cout<<"treenode executing" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }
}

void TreeNodeImpl::setNodeVariables(const GameVariables &data) {
    nodeVariables = data;
}

GameVariables TreeNodeImpl::getNodeVariables() const {
    return nodeVariables;
}

std::string TreeNodeImpl::getMessage(){
    // Temporary function for getting a message
    // Will be changed to use Lex's message queue
    return "string";
}

void TreeNodeImpl::eraseMessage(){
    // TODO 
    // Implement this will the queue
}

ForNodeImpl::ForNodeImpl(std::string id, GameState* _gameState): TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "for"})");
    GameVariables gv;
    gv.insert("_type", std::string("for"));
    nodeVariables = gv;
}

void ForNodeImpl::execute(){
    // json identifierMap = getIdentifierData();
    // json* gameStateMap = getGameStateData();
    // auto array = identifierMap[TreeNodeImpl::COLLECTION_ID];

    // auto freshVariable = identifierMap[TreeNodeImpl::VARIABLE_ID].dump();
    // for (auto el: array) {
    //     if(array.is_array()){
    //         (*gameStateMap)[freshVariable] = el;
    //     }
        
        for (const auto& child : children) {
            child->execute();
        }
    // }
    // identifierMap.erase(freshVariable);
    
}

DiscardNodeImpl::DiscardNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "discard"})");
    GameVariables gv;
    gv.insert("_type", std::string("discard"));
    nodeVariables = gv;
}

// Checking if the array should be fully wiped
// Appreciate feedback on this
bool DiscardNodeImpl::extractSize(const std::string_view operand){
    return  std::string_view::npos != operand.find(".size()");
}

void DiscardNodeImpl::execute(){
    // //std::cout<< "executing discard" <<std::endl;

    // auto gameData = getGameStateData();
    // auto idData = getIdentifierData();

    // // TODO: Change json to a map format
    // auto arrayId = idData[TreeNodeImpl::VARIABLE_ID].get<std::string>();
    // auto array = (*gameData)[arrayId];
    // auto size = idData[TreeNodeImpl::OPERAND_ID].dump();

    // // TODO: account for when only a portion of array should be wiped
    // if(extractSize(size)){
    //     (*gameData)[arrayId].clear();
    // }

    for (const auto& child : children) {
        child->execute();
    }
}

MessageNodeImpl::MessageNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "message"})");
    GameVariables gv;
    gv.insert("_type", std::string("message"));
    nodeVariables = gv;
}

void MessageNodeImpl::execute(){
   // std::cout<< "executing message" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: once Arjun has the server I/O interface ready, call it here to send the message
}

ParallelForNodeImpl::ParallelForNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "parallel_for"})");
    GameVariables gv;
    gv.insert("_type", std::string("parallel_for"));
    nodeVariables = gv;
}

void ParallelForNodeImpl::execute(){
    auto idVars = getNodeVariables();
    auto freshID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::VARIABLE_ID));
    auto collectionID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::COLLECTION_ID));

    auto gameVars = gameState->getVars();
    auto collection = gameVars->getNestedMap(collectionID);

    // Map of what to do after executing a node in parallel
    std::unordered_map<std::string, std::function<void(ParallelForNodeImpl*, const size_t& size, const std::string& )>> visitParallelMap;
    visitParallelMap["input_choice"] = visitParallelInput;

    // Only works with inputNodes
    // Can be refactored to work when a child is an input node 
    std::visit([this, &gameVars, &freshID, &visitParallelMap](const auto& value) {
        // First check if its an array
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::vector<ArrayType>>) {
            // Pass all elements into the temp variable so the child can use
            for(const auto& el: value){
                gameVars->insert(freshID, el);
                for (const auto& child : children) {
                    child->execute();
                }
            }

            // Depending on what child is what handle the outcomes
            // Right now only inputNode is programmed into the map
            for (const auto& child : children){
                (visitParallelMap[child->getType()](this, value.size(), freshID));
            }
            
        } 
    }, collection);
}

InputChoiceNodeImpl::InputChoiceNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "input_choice"})");
    GameVariables gv;
    gv.insert("_type", std::string("input_choice"));
    nodeVariables = gv;
}

using json = nlohmann::json;

enum MessageType { SEND, RESPONSE };
// tempory message type before Lex adds updates the message struct in networking
struct Message {
    MessageType type;
    std::vector<int> choices;
    std::string prompt;
    int playerID;

    void to_json(json& j) const {
        j = json{
            {"type", type},
            {"choices", choices},
            {"prompt", prompt},
            {"playerID", playerID}
        };
    }

    // used for debugging to print the contents of a Message type. 
    std::string stringify() {
        json jsonOutput;
        this->to_json(jsonOutput);
        return jsonOutput.dump();
    }
};


void InputChoiceNodeImpl::execute(){

    auto idVars = getNodeVariables();

    auto targetID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::TARGET_ID));
    auto promptID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::PROMPT_ID));
    auto choicesID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::CHOICES_ID));

    auto gameVars = gameState->getVars();
    auto choices = gameVars->getNestedMap(choicesID);
    auto target = gameVars->getNestedMap(targetID);
    auto prompt = gameVars->getNestedMap(promptID);

    Message InputMessage;
    int intTarget;
    std::string stringPrompt;
    std::vector<int> intChoices;

    std::visit([&prompt, &stringPrompt](auto&& e){
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, std::string> ){
                stringPrompt = std::get<std::string>(prompt);
            }
        }, prompt);

 
    std::visit([&target, &intTarget](auto&& e){
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, int> ){
                intTarget = std::get<int>(target);
            }
        }, target);

    std::visit([&choices, &intChoices ](auto&& elements){
        using U = std::decay_t<decltype(elements)>;
        if constexpr (std::is_same_v<U, std::vector<ArrayType>> ){
            for(auto& el: elements){
                std::visit([&el, &intChoices](auto&& e){
                    using T = std::decay_t<decltype(e)>;
                    if constexpr (std::is_same_v<T, int> ){
                        auto intChoice = std::get<int>(el);
                        intChoices.emplace_back(intChoice);
                    }
                }, el);
            }
        }
    }, choices);

    InputMessage = Message{ MessageType{SEND}, intChoices, stringPrompt, intTarget };
    //const int PORT_ID = 8888;
    
    // for testing:
    std::cout << InputMessage.stringify() << std::endl;
    
    // Adding to the queue will look something like this. Queue is not yet instantiated globally

    // networking::Connection con{PORT_ID};
    // networking::Message networkingMessage{con, serializeMessage(InputMessage)};
    // MessageQueue.add(networkingMessage);


    for (const auto& child : children) {
        child->execute();
    }
}

ScoresNodeImpl::ScoresNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "scores"})");
    GameVariables gv;
    gv.insert("_type", std::string("scores"));
    nodeVariables = gv;
}

void ScoresNodeImpl::execute(){
    //std::cout<< "executing scores" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: get scores from game state, sort it, then message it to everyone
}

AssignmentNodeImpl::AssignmentNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "assignment"})");
    GameVariables gv;
    gv.insert("_type", std::string("assignment"));
    nodeVariables = gv;
}

void AssignmentNodeImpl::execute(){
   // std::cout<< "executing assignment" <<std::endl;

    // auto gameData = getGameStateData();
    // auto idData = getIdentifierData();

    // // TODO: Change json to a map format
    // auto targetId = idData[TreeNodeImpl::TARGET_ID].get<std::string>();
    // auto value = idData[TreeNodeImpl::VALUE_ID].dump();

    // // TODO: figure out variants and mapping
    // // Currently just assign value
    // (*gameData)[targetId] = value;

    for (const auto& child : children) {
        child->execute();
    }
}

MatchNodeImpl::MatchNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "match"})");
    GameVariables gv;
    gv.insert("_type", std::string("match"));
    nodeVariables = gv;
}

void MatchNodeImpl::execute(){
   // std::cout<< "executing match" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: evaluate the condition and then write it to the game state, to be compared against the match_entry nodes
}

MatchEntryNodeImpl::MatchEntryNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "match_entry"})");
    GameVariables gv;
    gv.insert("_type", std::string("match_entry"));
    nodeVariables = gv;
}

void MatchEntryNodeImpl::execute(){
   // std::cout<< "executing match entry" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: evaluate the condition and then compare it to the game state's stored match expression
}

ExtendNodeImpl::ExtendNodeImpl(std::string id, GameState* _gameState) : TreeNodeImpl(id, _gameState) {
    identifiers = json::parse(R"({"_type": "extend"})");
    GameVariables gv;
    gv.insert("_type", std::string("extend"));
    nodeVariables = gv;
}

void ExtendNodeImpl::execute(){
    //std::cout<< "executing extend" <<std::endl;
    for (const auto& child : children) {
        child->execute();
    }

    //TODO: get the collection from the game state, append the list or element onto it, then write it again
}
