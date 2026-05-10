/*
 * tree_node.h  —  Concrete parse-tree node shared between the LL(1) parser
 *                 and the AST builder.
 */

#ifndef MINI_TREE_NODE_H
#define MINI_TREE_NODE_H

#include <string>
#include <vector>
#include <memory>

struct TreeNode {
    std::string label;
    std::vector<std::shared_ptr<TreeNode>> children;

    explicit TreeNode(std::string lbl) : label(std::move(lbl)) {}

    void addChild(std::shared_ptr<TreeNode> c)
    {
        children.push_back(std::move(c));
    }
};

void printTree(const std::shared_ptr<TreeNode> &node,
               const std::string &prefix = "",
               bool isLast = true);

#endif // MINI_TREE_NODE_H
