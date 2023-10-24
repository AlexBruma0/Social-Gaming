#include "ruleNode.h"
#include "ruleNodeSupport.h"



/*
-------------------------------------------
TREE NODE CLASS 
-------------------------------------------
*/

TreeNode::TreeNode(std::string node, std::string type) : nodeType(type) {
    impl = std::move(this->parseNode(node));
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

std::unique_ptr<TreeNodeImpl> TreeNode::parseNode(const std::string& node){
    std::unordered_map<std::string, std::function<std::unique_ptr<TreeNodeImpl>(const std::string&)>> typeToFunction;
    typeToFunction["for"] = processFor;
    typeToFunction["discard"] = processDiscard;
//    typeToFunction["message"] = processMessage;
//    typeToFunction["parallel_for"] = processParallelFor;
//    typeToFunction["input_choice"] = processInputChoice;
//    typeToFunction["match"] = processMatch;
//    typeToFunction["scores"] = processScores;
//    typeToFunction["extend"] = processExtend;

    if (typeToFunction.count(nodeType) > 0) {
        return typeToFunction[nodeType](node);
    }
    return std::make_unique<TreeNodeImpl>("bad");
}

void TreeNode::execute() const{
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

void TreeNodeImpl::execute(){
    std::for_each(children.begin(), children.end(), [](const TreeNode* child){
        child->execute();
    });
}

void ForNodeImpl::execute(){
    std::cout<< "executing for" <<std::endl;
    std::for_each(children.begin(), children.end(), [](const TreeNode* child){
        child->execute();
    });
}

void DiscardNodeImpl::execute(){
    std::cout<< "executing discard" <<std::endl;
    std::for_each(children.begin(), children.end(), [](const TreeNode* child){
        child->execute();
    });
}
