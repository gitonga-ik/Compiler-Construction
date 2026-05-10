/*
 * ast_builder.h  --  Converts a parse-tree (TreeNode) to an AST.
 */

#ifndef MINI_AST_BUILDER_H
#define MINI_AST_BUILDER_H

#include <memory>
#include <string>
#include <vector>
#include "ast.h"
#include "tree_node.h"

class ASTBuilder {
public:
    ASTNodePtr build(const std::shared_ptr<TreeNode> &root);

private:
    std::vector<ASTNodePtr> buildStmtList  (const std::shared_ptr<TreeNode> &node);
    std::vector<ASTNodePtr> buildStmtListP (const std::shared_ptr<TreeNode> &node);
    ASTNodePtr              buildStmt      (const std::shared_ptr<TreeNode> &node);
    ASTNodePtr              buildAssign    (const std::shared_ptr<TreeNode> &node);
    ASTNodePtr              buildPrint     (const std::shared_ptr<TreeNode> &node);
    ASTNodePtr              buildIf        (const std::shared_ptr<TreeNode> &node);
    std::vector<ASTNodePtr> buildElsePart  (const std::shared_ptr<TreeNode> &node);
    ASTNodePtr              buildWhile     (const std::shared_ptr<TreeNode> &node);

    ASTNodePtr buildExpr      (const std::shared_ptr<TreeNode> &node);
    ASTNodePtr buildExprP     (const std::shared_ptr<TreeNode> &node, ASTNodePtr lhs);
    ASTNodePtr buildArithExpr (const std::shared_ptr<TreeNode> &node);
    ASTNodePtr buildArithExprP(const std::shared_ptr<TreeNode> &node, ASTNodePtr lhs);
    ASTNodePtr buildTerm      (const std::shared_ptr<TreeNode> &node);

    static std::string extractLexeme(const std::string &label);
    static std::shared_ptr<TreeNode> findChild(const std::shared_ptr<TreeNode> &parent,
                                                const std::string &prefix);
};

#endif // MINI_AST_BUILDER_H
