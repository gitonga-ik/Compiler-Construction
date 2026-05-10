/*
 * ast.cpp  —  AST pretty-printer (tree-view with box-drawing characters).
 */

#include <iostream>
#include <string>
#include "ast.h"

// ── helpers ───────────────────────────────────────────────────────────────────

static void printNodeHeader(const std::string &label,
                            const std::string &prefix,
                            bool isLast)
{
    if (prefix.empty()) {
        std::cout << label << "\n";
    } else {
        std::cout << prefix << (isLast ? "`-- " : "|-- ") << label << "\n";
    }
}

static std::string childPrefix(const std::string &prefix, bool isLast)
{
    return prefix + (isLast ? "    " : "|   ");
}

static void printChildren(const std::vector<ASTNodePtr> &children,
                          const std::string &pfx)
{
    for (size_t i = 0; i < children.size(); ++i)
        printAST(children[i], pfx, i == children.size() - 1);
}

// ── public entry ─────────────────────────────────────────────────────────────

void printAST(const ASTNodePtr &node, const std::string &prefix, bool isLast)
{
    if (!node) return;

    switch (node->kind) {

    case ASTKind::Program: {
        auto &n = static_cast<ProgramNode &>(*node);
        printNodeHeader("Program", prefix, isLast);
        printChildren(n.stmts, childPrefix(prefix, isLast));
        break;
    }

    case ASTKind::Assign: {
        auto &n = static_cast<AssignNode &>(*node);
        std::string lbl = "Assign: " + n.name;
        if (!n.type.empty()) lbl += "  [decl: " + n.type + "]";
        printNodeHeader(lbl, prefix, isLast);
        std::string pfx = childPrefix(prefix, isLast);
        printAST(n.expr, pfx, true);
        break;
    }

    case ASTKind::Print: {
        auto &n = static_cast<PrintNode &>(*node);
        printNodeHeader("Print", prefix, isLast);
        printAST(n.expr, childPrefix(prefix, isLast), true);
        break;
    }

    case ASTKind::If: {
        auto &n = static_cast<IfNode &>(*node);
        bool hasElse = !n.elseBody.empty();
        printNodeHeader("If", prefix, isLast);
        std::string pfx = childPrefix(prefix, isLast);

        // condition child
        printNodeHeader("Condition", pfx, false);
        printAST(n.cond, childPrefix(pfx, false), true);

        // then body
        printNodeHeader("Then", pfx, !hasElse);
        printChildren(n.body, childPrefix(pfx, !hasElse));

        // optional else body
        if (hasElse) {
            printNodeHeader("Else", pfx, true);
            printChildren(n.elseBody, childPrefix(pfx, true));
        }
        break;
    }

    case ASTKind::While: {
        auto &n = static_cast<WhileNode &>(*node);
        printNodeHeader("While", prefix, isLast);
        std::string pfx = childPrefix(prefix, isLast);
        printNodeHeader("Condition", pfx, false);
        printAST(n.cond, childPrefix(pfx, false), true);
        printNodeHeader("Body", pfx, true);
        printChildren(n.body, childPrefix(pfx, true));
        break;
    }

    case ASTKind::BinOp: {
        auto &n = static_cast<BinOpNode &>(*node);
        printNodeHeader("BinOp(" + n.op + ")", prefix, isLast);
        std::string pfx = childPrefix(prefix, isLast);
        printAST(n.left,  pfx, false);
        printAST(n.right, pfx, true);
        break;
    }

    case ASTKind::Id: {
        auto &n = static_cast<IdNode &>(*node);
        printNodeHeader("Id(" + n.name + ")", prefix, isLast);
        break;
    }

    case ASTKind::IntLit: {
        auto &n = static_cast<IntLitNode &>(*node);
        printNodeHeader("IntLit(" + std::to_string(n.value) + ")", prefix, isLast);
        break;
    }

    case ASTKind::StrLit: {
        auto &n = static_cast<StrLitNode &>(*node);
        printNodeHeader("StrLit(" + n.value + ")", prefix, isLast);
        break;
    }

    } // switch
}
