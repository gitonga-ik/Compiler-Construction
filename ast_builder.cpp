/*
 * ast_builder.cpp  —  Converts the concrete parse tree to an AST.
 *
 * The parse-tree labels follow the convention set in ll1_parser.cpp:
 *   • Non-terminal nodes:  "<program>", "<stmt_list>", "<expr'>", …
 *   • Terminal leaves:     "BEGIN", "id(x)", "integer(42)", "=", …
 *   • Epsilon nodes:       "epsilon"
 *
 * The builder relies only on those label strings; it does not re-parse
 * the token stream.
 */

#include <stdexcept>
#include <algorithm>
#include "ast_builder.h"

// ── utilities ─────────────────────────────────────────────────────────────────

std::string ASTBuilder::extractLexeme(const std::string &label)
{
    auto p = label.find('(');
    if (p == std::string::npos) return label;
    auto q = label.rfind(')');
    if (q == std::string::npos || q <= p) return label;
    return label.substr(p + 1, q - p - 1);
}

std::shared_ptr<TreeNode>
ASTBuilder::findChild(const std::shared_ptr<TreeNode> &parent,
                      const std::string &prefix)
{
    for (auto &c : parent->children)
        if (c->label.find(prefix) == 0)
            return c;
    return nullptr;
}

// ── entry point ───────────────────────────────────────────────────────────────

ASTNodePtr ASTBuilder::build(const std::shared_ptr<TreeNode> &root)
{
    // root label is "Program"; its children are BEGIN <stmt_list> END
    // We need the <stmt_list> child.
    auto stmtListNode = findChild(root, "<stmt_list>");
    if (!stmtListNode)
        throw std::runtime_error("[ASTBuilder] Cannot find <stmt_list> under Program root");

    auto prog = std::make_shared<ProgramNode>();
    prog->stmts = buildStmtList(stmtListNode);
    return prog;
}

// ── stmt_list → stmt stmt_list' ───────────────────────────────────────────────

std::vector<ASTNodePtr>
ASTBuilder::buildStmtList(const std::shared_ptr<TreeNode> &node)
{
    // node has two children: <stmt>  <stmt_list'>
    std::vector<ASTNodePtr> stmts;

    auto stmtNode = findChild(node, "<stmt>");
    if (stmtNode) {
        auto s = buildStmt(stmtNode);
        if (s) stmts.push_back(s);
    }

    auto primeNode = findChild(node, "<stmt_list'>");
    if (primeNode) {
        auto rest = buildStmtListP(primeNode);
        stmts.insert(stmts.end(), rest.begin(), rest.end());
    }
    return stmts;
}

// ── stmt_list' → stmt stmt_list'  |  ε ───────────────────────────────────────

std::vector<ASTNodePtr>
ASTBuilder::buildStmtListP(const std::shared_ptr<TreeNode> &node)
{
    if (node->children.empty()) return {};
    if (node->children.size() == 1 && node->children[0]->label == "epsilon")
        return {};

    std::vector<ASTNodePtr> stmts;

    auto stmtNode = findChild(node, "<stmt>");
    if (stmtNode) {
        auto s = buildStmt(stmtNode);
        if (s) stmts.push_back(s);
    }

    auto primeNode = findChild(node, "<stmt_list'>");
    if (primeNode) {
        auto rest = buildStmtListP(primeNode);
        stmts.insert(stmts.end(), rest.begin(), rest.end());
    }
    return stmts;
}

// ── stmt → assignment | print_stmt | if_stmt | while_stmt ────────────────────

ASTNodePtr ASTBuilder::buildStmt(const std::shared_ptr<TreeNode> &node)
{
    // node has exactly one child: the concrete statement non-terminal
    if (node->children.empty()) return nullptr;
    auto &child = node->children[0];

    if (child->label == "<assignment>") return buildAssign(child);
    if (child->label == "<print_stmt>") return buildPrint(child);
    if (child->label == "<if_stmt>")    return buildIf(child);
    if (child->label == "<while_stmt>") return buildWhile(child);

    throw std::runtime_error("[ASTBuilder] Unknown stmt child: " + child->label);
}

// ── assignment → type id = expr ;  |  id = expr ; ────────────────────────────

ASTNodePtr ASTBuilder::buildAssign(const std::shared_ptr<TreeNode> &node)
{
    std::string type;
    std::string name;
    std::shared_ptr<TreeNode> exprNode;

    // Determine whether we have a type declaration
    if (!node->children.empty() && node->children[0]->label == "<type>") {
        // type → int | string
        auto typeNode = node->children[0];
        if (!typeNode->children.empty())
            type = typeNode->children[0]->label; // "int" or "string"

        // children: <type>  id(x)  =  <expr>  ;
        if (node->children.size() >= 4) {
            name     = extractLexeme(node->children[1]->label);
            exprNode = findChild(node, "<expr>");
        }
    } else {
        // children: id(x)  =  <expr>  ;
        if (!node->children.empty())
            name = extractLexeme(node->children[0]->label);
        exprNode = findChild(node, "<expr>");
    }

    if (!exprNode)
        throw std::runtime_error("[ASTBuilder] No <expr> in assignment for: " + name);

    return std::make_shared<AssignNode>(type, name, buildExpr(exprNode));
}

// ── print_stmt → cout << expr ; ──────────────────────────────────────────────

ASTNodePtr ASTBuilder::buildPrint(const std::shared_ptr<TreeNode> &node)
{
    auto exprNode = findChild(node, "<expr>");
    if (!exprNode)
        throw std::runtime_error("[ASTBuilder] No <expr> in print_stmt");
    return std::make_shared<PrintNode>(buildExpr(exprNode));
}

// ── if_stmt → if ( expr ) { stmt_list } else_part ────────────────────────────

ASTNodePtr ASTBuilder::buildIf(const std::shared_ptr<TreeNode> &node)
{
    auto exprNode     = findChild(node, "<expr>");
    auto stmtListNode = findChild(node, "<stmt_list>");
    auto elsePartNode = findChild(node, "<else_part>");

    if (!exprNode || !stmtListNode)
        throw std::runtime_error("[ASTBuilder] Malformed if_stmt");

    auto cond     = buildExpr(exprNode);
    auto body     = buildStmtList(stmtListNode);
    auto elseBody = elsePartNode ? buildElsePart(elsePartNode)
                                 : std::vector<ASTNodePtr>{};

    return std::make_shared<IfNode>(std::move(cond), std::move(body),
                                    std::move(elseBody));
}

// ── else_part → else { stmt_list }  |  ε ─────────────────────────────────────

std::vector<ASTNodePtr>
ASTBuilder::buildElsePart(const std::shared_ptr<TreeNode> &node)
{
    if (node->children.empty()) return {};
    if (node->children.size() == 1 && node->children[0]->label == "epsilon")
        return {};

    auto stmtListNode = findChild(node, "<stmt_list>");
    if (!stmtListNode) return {};
    return buildStmtList(stmtListNode);
}

// ── while_stmt → while ( expr ) { stmt_list } ────────────────────────────────

ASTNodePtr ASTBuilder::buildWhile(const std::shared_ptr<TreeNode> &node)
{
    auto exprNode     = findChild(node, "<expr>");
    auto stmtListNode = findChild(node, "<stmt_list>");

    if (!exprNode || !stmtListNode)
        throw std::runtime_error("[ASTBuilder] Malformed while_stmt");

    return std::make_shared<WhileNode>(buildExpr(exprNode),
                                       buildStmtList(stmtListNode));
}

// ── expr → arith_expr expr' ───────────────────────────────────────────────────

ASTNodePtr ASTBuilder::buildExpr(const std::shared_ptr<TreeNode> &node)
{
    auto arithNode = findChild(node, "<arith_expr>");
    auto primeNode = findChild(node, "<expr'>");

    if (!arithNode)
        throw std::runtime_error("[ASTBuilder] No <arith_expr> in expr");

    ASTNodePtr lhs = buildArithExpr(arithNode);

    if (primeNode)
        lhs = buildExprP(primeNode, lhs);

    return lhs;
}

// ── expr' → OP arith_expr  |  ε ──────────────────────────────────────────────

ASTNodePtr ASTBuilder::buildExprP(const std::shared_ptr<TreeNode> &node,
                                   ASTNodePtr lhs)
{
    if (node->children.empty()) return lhs;
    if (node->children.size() == 1 && node->children[0]->label == "epsilon")
        return lhs;

    // First child is the operator terminal: "==", "<", ">"
    std::string op = node->children[0]->label;

    // Second child is <arith_expr>
    ASTNodePtr rhs;
    for (auto &c : node->children)
        if (c->label == "<arith_expr>") { rhs = buildArithExpr(c); break; }

    if (!rhs) throw std::runtime_error("[ASTBuilder] No RHS in expr'");
    return std::make_shared<BinOpNode>(op, std::move(lhs), std::move(rhs));
}

// ── arith_expr → term arith_expr' ────────────────────────────────────────────

ASTNodePtr ASTBuilder::buildArithExpr(const std::shared_ptr<TreeNode> &node)
{
    auto termNode  = findChild(node, "<term>");
    auto primeNode = findChild(node, "<arith_expr'>");

    if (!termNode)
        throw std::runtime_error("[ASTBuilder] No <term> in arith_expr");

    ASTNodePtr lhs = buildTerm(termNode);

    if (primeNode)
        lhs = buildArithExprP(primeNode, lhs);

    return lhs;
}

// ── arith_expr' → + term arith_expr'  |  - term arith_expr'  |  ε ───────────

ASTNodePtr ASTBuilder::buildArithExprP(const std::shared_ptr<TreeNode> &node,
                                        ASTNodePtr lhs)
{
    if (node->children.empty()) return lhs;
    if (node->children.size() == 1 && node->children[0]->label == "epsilon")
        return lhs;

    // Children: OP  <term>  <arith_expr'>
    std::string op = node->children[0]->label;  // "+" or "-"

    ASTNodePtr termVal;
    std::shared_ptr<TreeNode> nextPrime;
    for (auto &c : node->children) {
        if (c->label == "<term>")         termVal    = buildTerm(c);
        if (c->label == "<arith_expr'>")  nextPrime  = c;
    }
    if (!termVal) throw std::runtime_error("[ASTBuilder] No <term> in arith_expr'");

    ASTNodePtr combined = std::make_shared<BinOpNode>(op, std::move(lhs),
                                                       std::move(termVal));
    if (nextPrime)
        return buildArithExprP(nextPrime, std::move(combined));
    return combined;
}

// ── term → id | integer | string_literal | ( expr ) ──────────────────────────

ASTNodePtr ASTBuilder::buildTerm(const std::shared_ptr<TreeNode> &node)
{
    if (node->children.empty())
        throw std::runtime_error("[ASTBuilder] Empty <term>");

    auto &child = node->children[0];
    const std::string &lbl = child->label;

    if (lbl.find("id(") == 0)
        return std::make_shared<IdNode>(extractLexeme(lbl));

    if (lbl.find("integer(") == 0) {
        int v = std::stoi(extractLexeme(lbl));
        return std::make_shared<IntLitNode>(v);
    }

    if (lbl.find("string_literal(") == 0)
        return std::make_shared<StrLitNode>(extractLexeme(lbl));

    if (lbl == "(") {
        // ( expr )  — find the <expr> among children of term
        auto exprNode = findChild(node, "<expr>");
        if (!exprNode)
            throw std::runtime_error("[ASTBuilder] No <expr> inside parenthesised term");
        return buildExpr(exprNode);
    }

    throw std::runtime_error("[ASTBuilder] Unknown term child: " + lbl);
}
