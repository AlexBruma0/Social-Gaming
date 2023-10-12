/*
 Created by kevin on 10/11/2023.

 Edited:
    cstong (Caleb) 10/12/2023 
*/


#include "ruleNode.h"

/*
-------------------------------------------
NODE TRIAT CLASSES
-------------------------------------------
*/

// define a trait for Node behaviour
template <typename T>
void NodeTrait<T>::execute(const T& node) {
    // Default behavior (do nothing)
}
template <typename T>
T NodeTrait<T>::parse(const std::string& data) {
    // Default behavior (return an empty node)
    return T();
}

template <typename T>
void TreeNode<T>::printTree(int depth) const {
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    std::cout << value << std::endl;

    for (const TreeNode& child : children) {
        child.printTree(depth + 1);
    }
}



/*
-------------------------------------------
TREE BASE CLASS 
-------------------------------------------
*/

template <typename T>
void TreeNode<T>::execute() {
    NodeTrait<T>::execute(static_cast<const T&>(*this));
}

// Parse data to create a node
template <typename T>
T TreeNode<T>::parse(const std::string& data) {
    return NodeTrait<T>::parse(data);
}

template <typename T>
void TreeNode<T>::addChild(const TreeNode<T>& child) {
    children.push_back(child);
}



/*
-------------------------------------------
RULE NODE CLASSES
-------------------------------------------
*/

// Needs to call base class constructor to populate and use the value field
template <typename T>
RuleNode<T>::RuleNode(const std::string& value) : TreeNode<T>(value) {};



// (cstong) TEMP COMMENTED OUT

/*
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