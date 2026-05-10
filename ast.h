/*
 * ast.h  —  Abstract Syntax Tree node hierarchy.
 *
 * The AST strips away syntactic scaffolding (keywords used only for
 * disambiguation, redundant grouping nodes, ε-productions) and retains
 * only semantically significant structure.
 *
 * Node kinds
 * ──────────
 *  ProgramNode         → root; holds a list of statements
 *  AssignNode          → [type?] id = expr
 *  PrintNode           → cout << expr
 *  IfNode              → if (cond) { body } [else { else_body }]
 *  WhileNode           → while (cond) { body }
 *  BinOpNode           → expr OP expr    (OP ∈ {+, -, ==, <, >})
 *  IdNode              → identifier leaf
 *  IntLitNode          → integer literal leaf
 *  StrLitNode          → string literal leaf
 */

#ifndef MINI_AST_H
#define MINI_AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// ── forward declarations ──────────────────────────────────────────────────────
struct ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

// ── visitor interface (forward-declared for use in nodes) ────────────────────
struct ASTVisitor;

// ── node kind tag ─────────────────────────────────────────────────────────────
enum class ASTKind {
    Program,
    Assign,
    Print,
    If,
    While,
    BinOp,
    Id,
    IntLit,
    StrLit
};

// ── base node ─────────────────────────────────────────────────────────────────
struct ASTNode {
    ASTKind kind;
    explicit ASTNode(ASTKind k) : kind(k) {}
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor &v) = 0;
};

// ── concrete nodes ────────────────────────────────────────────────────────────

struct ProgramNode : ASTNode {
    std::vector<ASTNodePtr> stmts;
    ProgramNode() : ASTNode(ASTKind::Program) {}
    void accept(ASTVisitor &v) override;
};

struct AssignNode : ASTNode {
    std::string type;   // "" if no type declaration (plain assignment)
    std::string name;
    ASTNodePtr  expr;
    AssignNode(std::string tp, std::string nm, ASTNodePtr ex)
        : ASTNode(ASTKind::Assign),
          type(std::move(tp)), name(std::move(nm)), expr(std::move(ex)) {}
    void accept(ASTVisitor &v) override;
};

struct PrintNode : ASTNode {
    ASTNodePtr expr;
    explicit PrintNode(ASTNodePtr ex)
        : ASTNode(ASTKind::Print), expr(std::move(ex)) {}
    void accept(ASTVisitor &v) override;
};

struct IfNode : ASTNode {
    ASTNodePtr              cond;
    std::vector<ASTNodePtr> body;
    std::vector<ASTNodePtr> elseBody;   // empty if no else-branch
    IfNode(ASTNodePtr c,
           std::vector<ASTNodePtr> b,
           std::vector<ASTNodePtr> eb)
        : ASTNode(ASTKind::If),
          cond(std::move(c)),
          body(std::move(b)),
          elseBody(std::move(eb)) {}
    void accept(ASTVisitor &v) override;
};

struct WhileNode : ASTNode {
    ASTNodePtr              cond;
    std::vector<ASTNodePtr> body;
    WhileNode(ASTNodePtr c, std::vector<ASTNodePtr> b)
        : ASTNode(ASTKind::While),
          cond(std::move(c)), body(std::move(b)) {}
    void accept(ASTVisitor &v) override;
};

struct BinOpNode : ASTNode {
    std::string op;     // "+", "-", "==", "<", ">"
    ASTNodePtr  left;
    ASTNodePtr  right;
    BinOpNode(std::string o, ASTNodePtr l, ASTNodePtr r)
        : ASTNode(ASTKind::BinOp),
          op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
    void accept(ASTVisitor &v) override;
};

struct IdNode : ASTNode {
    std::string name;
    explicit IdNode(std::string n)
        : ASTNode(ASTKind::Id), name(std::move(n)) {}
    void accept(ASTVisitor &v) override;
};

struct IntLitNode : ASTNode {
    int value;
    explicit IntLitNode(int v)
        : ASTNode(ASTKind::IntLit), value(v) {}
    void accept(ASTVisitor &v) override;
};

struct StrLitNode : ASTNode {
    std::string value;
    explicit StrLitNode(std::string v)
        : ASTNode(ASTKind::StrLit), value(std::move(v)) {}
    void accept(ASTVisitor &v) override;
};

// ── visitor interface ─────────────────────────────────────────────────────────
struct ASTVisitor {
    virtual void visit(ProgramNode &n) = 0;
    virtual void visit(AssignNode  &n) = 0;
    virtual void visit(PrintNode   &n) = 0;
    virtual void visit(IfNode      &n) = 0;
    virtual void visit(WhileNode   &n) = 0;
    virtual void visit(BinOpNode   &n) = 0;
    virtual void visit(IdNode      &n) = 0;
    virtual void visit(IntLitNode  &n) = 0;
    virtual void visit(StrLitNode  &n) = 0;
    virtual ~ASTVisitor() = default;
};

// ── accept implementations (inline; bodies defined after visitor) ─────────────
inline void ProgramNode::accept(ASTVisitor &v) { v.visit(*this); }
inline void AssignNode ::accept(ASTVisitor &v) { v.visit(*this); }
inline void PrintNode  ::accept(ASTVisitor &v) { v.visit(*this); }
inline void IfNode     ::accept(ASTVisitor &v) { v.visit(*this); }
inline void WhileNode  ::accept(ASTVisitor &v) { v.visit(*this); }
inline void BinOpNode  ::accept(ASTVisitor &v) { v.visit(*this); }
inline void IdNode     ::accept(ASTVisitor &v) { v.visit(*this); }
inline void IntLitNode ::accept(ASTVisitor &v) { v.visit(*this); }
inline void StrLitNode ::accept(ASTVisitor &v) { v.visit(*this); }

// ── pretty-printer ────────────────────────────────────────────────────────────
void printAST(const ASTNodePtr &node, const std::string &prefix = "",
              bool isLast = true);

#endif // MINI_AST_H
