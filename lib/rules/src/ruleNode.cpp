/*
 Created by kevin on 10/11/2023.

 Edited:
    cstong (Caleb) 10/12/2023
*/


#include "ruleNode.h"
#include "ruleNodeSupport.h"



/*
-------------------------------------------
TREE NODE CLASS 
-------------------------------------------
*/

TreeNode::TreeNode(std::string node, std::string type){
    impl = std::move(this->parseNode(node, type));
}

TreeNode::TreeNode(TreeNode&& other) noexcept
    // transfer ownership to allow copying
    : impl(std::move(other.impl)) {
}

void TreeNode::addChild(const TreeNode* child) const {
    impl->addChild(child);
}

void TreeNode::printTree(int depth) const {
    impl->printTree(depth);
}

void TreeNode::updateIdentifier(const std::string& identifier){
    impl->updateIdentifier(identifier);
}

std::unique_ptr<TreeNodeImpl> TreeNode::parseNode(const std::string& node, const std::string type){
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> typeToFunction;
    typeToFunction["for"] = processFor;
    typeToFunction["discard"] = processDiscard;
    typeToFunction["message"] = processMessage;
    typeToFunction["parallel_for"] = processParallelFor;
    typeToFunction["input_choice"] = processInputChoice;
    typeToFunction["match"] = processMatch;
    typeToFunction["scores"] = processScores;
    typeToFunction["extend"] = processExtend;

    std::string result = "";
    if (typeToFunction.find(type) != typeToFunction.end()) {
        result = typeToFunction[type](node);
    }
    /*
        Might look something like

        if node is a for node
            return make_unique forNode
        else if node is a discard node
            return make_unique discardNode
    */
    return std::make_unique<TreeNodeImpl>(result);
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


TreeNodeImpl::TreeNodeImpl(std::string id)
: identifier(id) {
}

TreeNodeImpl::~TreeNodeImpl(){

}

void TreeNodeImpl::addChild(const TreeNode* child){
    children.push_back(child);
}

void TreeNodeImpl::updateIdentifier(const std::string& identifier){
    this->identifier = identifier;
}

void TreeNodeImpl::printTree(int depth) const{
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    std::cout << identifier << std::endl;

    for (const auto& child : children) {
        child->printTree(depth + 1);
    }

}

// execute will differ depending on what type of node is implemented 
// Test implementation for now
void RuleNode::execute(){
    //std::cout<< "impl executing" <<std::endl;
    std::for_each(children.begin(), children.end(), [](const TreeNode* child){
        child->execute();
    });
}


// Same as RuleNode Temp for testing
void ForNode::execute(){
    //std::cout<< "impl executing" <<std::endl;
    std::for_each(children.begin(), children.end(), [](const TreeNode* child){
        child->execute();
    });
}



// (cstong) TEMP COMMENTED OUT

/*

// Needs to call base class constructor to populate and use the value field
RuleNode::RuleNode() : TreeNode() {};

RuleNode::~RuleNode(){}


// Needs to call base class constructor to populate and use the value field
ForNode::ForNode() : TreeNode() {};

ForNode::~ForNode(){}

// define a trait for Node behaviour
template <typename T>
void NodeTrait<T>::execute(const T& node) {
    // Default behavior (do nothing)
    printf("test");
}
template <typename T>
T NodeTrait<T>::parse(const std::string& data) {
    // Default behavior (return an empty node)
    return T();
}


// templatization; inserting the implementations
template <>
struct NodeTrait<forNode> {
    static void execute(const forNode& node) {
        std::cout << "Execution for Node of type: for: " << std::endl;
        // not needed yet, but for example, in this "execution" we would need to:
        //      figure out the looping variable and write it to json
        //      figure out how many times we need to loop
        //      execute the children that many times (changing state as necessary between runs)
    }

    static forNode parse(const std::string& data) {
        // for example, if this is a for loop, then we want to do two things:
        //      first, check that it is the node with all the content (not just the word "for")
        //          we can do this by breaking the contents into tokens, and then checking for more than 1
        //          use the function in ruleNodeSupport for this.
        //      second, we should crop it so that only the RELEVANT stuff is here
        //          the for loop will also include everything inside it. We don't want that because it'll be its own node.
        //          thus, we should discard everything after the first line break (make a method in ruleNodeSupport)
        return forNode(validatedData);
    }
};

template <>
struct NodeTrait<discardNode> {
    static void execute(const discardNode& node) {
        std::cout << "Execution for Node of type: discard: " << std::endl;
        // the requirements for this execution are to simply modify the game state, thus we assume it has no children
    }

    static discardNode parse(const std::string& data) {
        // discard will have different validating requirements.
        //      in this case, we simply want to ensure that it includes the whole line. No need for cutting the string.
        //      just check that there's more than one token here.

        //  etc, every node type will have different validation requirements.
        return discardNode(validatedData);
    }
};

// generally, the GOAL is to make sure that we have nodes representing the control flow logically.
//      we should validate this by tokenizing them and making sure they have enough information, then making sure there isn't overlap.
//      stuff like for or parallel for will naturally overlap because they contain stuff after the condition... discard it
//      hopefully, you can eventually represent the rules node by node cleanly... with only the following types and no overlap.
//          "for", "loop", "parallel_for", "in_parallel", "match", "extend", "reverse", "shuffle",
//          "sort", "deal", "discard", "assignment", "timer", "input_choice", "input_text", "input_vote",
//          "input_range", "message", "scores"

*/