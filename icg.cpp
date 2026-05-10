/*
 * icg.cpp  —  ICG visitor implementation.
 */

#include "icg.h"
#include <stdexcept>

// ── entry point ───────────────────────────────────────────────────────────────

std::vector<TACInstr> ICG::generate(ASTNodePtr &root)
{
    code_.clear();
    result_.clear();
    tempCnt_  = 0;
    labelCnt_ = 0;

    root->accept(*this);
    return std::move(code_);
}

// ── helper: evaluate an expression node, return result name ──────────────────

std::string ICG::evalExpr(ASTNode &node)
{
    node.accept(*this);
    return result_;
}

// ── program ───────────────────────────────────────────────────────────────────

void ICG::visit(ProgramNode &n)
{
    for (auto &stmt : n.stmts)
        stmt->accept(*this);
}

// ── assignment ────────────────────────────────────────────────────────────────

void ICG::visit(AssignNode &n)
{
    std::string rhs = evalExpr(*n.expr);
    emit(TACInstr::copy(n.name, rhs));
}

// ── print ─────────────────────────────────────────────────────────────────────

void ICG::visit(PrintNode &n)
{
    std::string val = evalExpr(*n.expr);
    emit(TACInstr::print(val));
}

// ── if / else ─────────────────────────────────────────────────────────────────

void ICG::visit(IfNode &n)
{
    std::string tCond = evalExpr(*n.cond);

    std::string lElse = freshLabel();
    std::string lEnd  = freshLabel();

    // IFFALSE tCond GOTO lElse
    emit(TACInstr::ifFalse(tCond, lElse));

    // then body
    for (auto &s : n.body)
        s->accept(*this);

    // GOTO lEnd  (skip else)
    emit(TACInstr::gotoLabel(lEnd));

    // lElse:
    emit(TACInstr::label(lElse));

    // else body (may be empty)
    for (auto &s : n.elseBody)
        s->accept(*this);

    // lEnd:
    emit(TACInstr::label(lEnd));
}

// ── while ─────────────────────────────────────────────────────────────────────

void ICG::visit(WhileNode &n)
{
    std::string lStart = freshLabel();
    std::string lEnd   = freshLabel();

    // lStart:
    emit(TACInstr::label(lStart));

    std::string tCond = evalExpr(*n.cond);

    // IFFALSE tCond GOTO lEnd
    emit(TACInstr::ifFalse(tCond, lEnd));

    // loop body
    for (auto &s : n.body)
        s->accept(*this);

    // GOTO lStart
    emit(TACInstr::gotoLabel(lStart));

    // lEnd:
    emit(TACInstr::label(lEnd));
}

// ── binary operation ──────────────────────────────────────────────────────────

void ICG::visit(BinOpNode &n)
{
    std::string lhs = evalExpr(*n.left);
    std::string rhs = evalExpr(*n.right);
    std::string tmp = freshTemp();
    emit(TACInstr::binaryOp(tmp, lhs, n.op, rhs));
    result_ = tmp;
}

// ── identifier leaf ───────────────────────────────────────────────────────────

void ICG::visit(IdNode &n)
{
    result_ = n.name;
}

// ── integer literal ───────────────────────────────────────────────────────────

void ICG::visit(IntLitNode &n)
{
    result_ = std::to_string(n.value);
}

// ── string literal ────────────────────────────────────────────────────────────

void ICG::visit(StrLitNode &n)
{
    // Keep the surrounding quotes so it is distinguishable in the TAC output.
    result_ = n.value;
}
