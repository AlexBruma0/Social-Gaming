/*
 Created by kevin on 10/11/2023.

 Edited:
    cstong (Caleb) 10/12/2023
*/

#include "parser.h"
#include "ruleNode.h"
#include "ruleNodeSupport.h"
#include <memory>
#include <nlohmann/json.hpp>
#include "Server.h"




/*
-------------------------------------------
TREE NODE CLASS 
-------------------------------------------
*/

TreeNode::TreeNode(const ts::Node& tsNode, std::string type, const std::string& sourceCode,
                   GameState* gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : nodeType(type) {
    impl = std::move(this->parseNode(tsNode, gameState, sourceCode, in, out));
    // TODO: put the queues in parse, then put those in the impl
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

std::unique_ptr<TreeNodeImpl> TreeNode::parseNode(const ts::Node tsNode, GameState* gameState,const std::string& source_code, const  SendMessageQueue* in, const ReceiveMessageQueue* out) {
    std::unordered_map<std::string, std::function<std::unique_ptr<TreeNodeImpl>(const ts::Node&, GameState*, const std::string&, const SendMessageQueue* , const ReceiveMessageQueue* )>> typeToFunction;
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
        std::unique_ptr<TreeNodeImpl> impl = typeToFunction[nodeType](tsNode, gameState, source_code, in, out);
        //std::cout << std::endl << impl->getIdentifierData().dump();
        return std::move(impl);
    }
    return std::make_unique<TreeNodeImpl>("", gameState, in , out);
}

GameVariables TreeNode::getNodeVariables(){
    return impl->getNodeVariables();
}

void TreeNode::execute() const{
    //std::cout<< "executing" <<std::endl;
    impl->execute();
}

TreeNode& TreeNode::operator=(const TreeNode& other) {
    if (this != &other) {
        impl = std::make_unique<TreeNodeImpl>(std::move(*other.impl));
        nodeType = other.nodeType;
    }
    return *this;
}

/*
-------------------------------------------
NODE CLASSES
-------------------------------------------
*/


TreeNodeImpl::TreeNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* sq, const ReceiveMessageQueue* rq): content(id), gameState(_gameState), in(sq), out(rq) {
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

TreeNodeImpl::TreeNodeImpl(TreeNodeImpl&& other) noexcept
        : identifiers(std::move(other.identifiers)),
          nodeVariables(std::move(other.nodeVariables)),
          idIndexes(std::move(other.idIndexes)),
          children(std::move(other.children)),
          content(std::move(other.content)),
          gameState(other.gameState) {
    other.gameState = nullptr;
}

TreeNodeImpl& TreeNodeImpl::operator=(TreeNodeImpl&& other) noexcept {
    if (this != &other) {
        identifiers = std::move(other.identifiers);
        nodeVariables = std::move(other.nodeVariables);
        idIndexes = std::move(other.idIndexes);
        children = std::move(other.children);
        content = std::move(other.content);
        gameState = other.gameState;
        other.gameState = nullptr;
    }
    return *this;
}

ForNodeImpl::ForNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out): TreeNodeImpl(id, _gameState, in, out) {
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

DiscardNodeImpl::DiscardNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
    identifiers = json::parse(R"({"_type": "discard"})");
    GameVariables gv;
    gv.insert("_type", std::string("discard"));
    nodeVariables = gv;
}

// Checking if the array should be fully wiped
// Appreciate feedback on this
// bool DiscardNodeImpl::extractSize(const std::string_view operand){
//     return  std::string_view::npos != operand.find(".size()");
// }

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

MessageNodeImpl::MessageNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
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

ParallelForNodeImpl::ParallelForNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
    identifiers = json::parse(R"({"_type": "parallel_for"})");
    GameVariables gv;
    gv.insert("_type", std::string("parallel_for"));
    nodeVariables = gv;
}

void ParallelForNodeImpl::broadcastInputs(){
    // Place holder function for returning control back to the networking side
    // Will call a function on the networking portion to broadcast messages in the msesage queue

    std::cout<<"broadcasting"<<std::endl;
    this->gameState->getServer()->broadcastMessage();
}

void ParallelForNodeImpl::waitResponses(size_t duration, const std::vector<int>& choices){
    // Place holder function for returning control back to the networking side
    // Will call a function on the networking portion to recieve messages for a set duration

    std::cout<<"responding"<<std::endl;
    this->gameState->getServer()->awaitResponse(duration, choices);
}

void ParallelForNodeImpl::execute(){
    std::cout<< "parallel executing" <<std::endl;
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
            
            // Keep track of the different timeouts 
            // Uses the address of the childNode as a key
            std::map<TreeNode*, size_t> durationMap;

            // Once all the messages are in the queue
            // Give control to the networking side and send all those messages
            // Also get the waiting durations for all children
            for (const auto& child : children) {
                GameVariables childVars = child->getNodeVariables();
                size_t duration = 0;
                auto timeoutVariant = childVars.getNestedMap(TIMEOUT_ID);
                broadcastInputs();

                // Check to see if a child node has a timeout field
                // If it does convert and record it 
                std::visit([&duration](const auto& v){
                    using U = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<U, std::string>){
                        duration = std::stoi(v);
                    }
                }, timeoutVariant); 
                durationMap[(child.get())] = duration;
            }

            // Wait for messages from the server
            // Then execute a function based on the type
            // For input nodes it will write the response to the global game state
            for (const auto& child : children){
                GameVariables childVars = child->getNodeVariables();
                auto choicesID = std::get<std::string>(childVars.getNestedMap(TreeNodeImpl::CHOICES_ID));
                auto choices = gameVars->getNestedMap(choicesID);

                std::vector<int> intChoices;

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
                waitResponses(durationMap[(child.get())], intChoices);
                (visitParallelMap[child->getType()](this, value.size(), freshID));
            }
            
        } 
    }, collection);
}

InputChoiceNodeImpl::InputChoiceNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
    identifiers = json::parse(R"({"_type": "input_choice"})");
    GameVariables gv;
    gv.insert("_type", std::string("input_choice"));
    nodeVariables = gv;
}


void InputChoiceNodeImpl::execute(){
    std::cout<< "input executing" <<std::endl;
    auto idVars = getNodeVariables();

    auto targetID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::TARGET_ID));
    auto promptID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::PROMPT_ID));
    auto choicesID = std::get<std::string>(idVars.getNestedMap(TreeNodeImpl::CHOICES_ID));

    auto gameVars = gameState->getVars();
    auto choices = gameVars->getNestedMap(choicesID);
    auto target = gameVars->getNestedMap(targetID);
    auto prompt = gameVars->getNestedMap(promptID);

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

    networking::SendMessage InputMessage = networking::SendMessage{ intChoices, stringPrompt };
    
    //InputMessage.print();
    if(in != nullptr){
        in->add(InputMessage);
    }
    for (const auto& child : children) {
        child->execute();
    }
}

ScoresNodeImpl::ScoresNodeImpl(std::string id, GameState* _gameState,const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
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

AssignmentNodeImpl::AssignmentNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
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

MatchNodeImpl::MatchNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
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

MatchEntryNodeImpl::MatchEntryNodeImpl(std::string id, GameState* _gameState, const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
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

ExtendNodeImpl::ExtendNodeImpl(std::string id, GameState* _gameState,const SendMessageQueue* in, const ReceiveMessageQueue* out) : TreeNodeImpl(id, _gameState, in, out) {
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
