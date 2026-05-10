/*
 * icg.h  —  Intermediate Code Generator.
 *
 * The ICG walks the AST using the Visitor pattern and emits a flat
 * sequence of Three-Address Code (TAC) instructions.
 *
 * Naming conventions
 * ──────────────────
 *  Temporaries   : t0, t1, t2, …   (freshTemp())
 *  Labels        : L0, L1, L2, …   (freshLabel())
 *
 * Control-flow pattern for if/else
 * ─────────────────────────────────
 *   <eval cond into tCond>
 *   IFFALSE tCond GOTO Lelse
 *   <then body>
 *   GOTO Lend
 *  Lelse:
 *   <else body>          (may be empty)
 *  Lend:
 *
 * Control-flow pattern for while
 * ────────────────────────────────
 *  Lstart:
 *   <eval cond into tCond>
 *   IFFALSE tCond GOTO Lend
 *   <body>
 *   GOTO Lstart
 *  Lend:
 */

#ifndef MINI_ICG_H
#define MINI_ICG_H

#include <string>
#include <vector>
#include "ast.h"
#include "tac.h"

class ICG : public ASTVisitor {
public:
    // Run the generator and return the TAC instruction sequence.
    std::vector<TACInstr> generate(ASTNodePtr &root);

    // ── visitor overrides ─────────────────────────────────────────────────────
    void visit(ProgramNode &n) override;
    void visit(AssignNode  &n) override;
    void visit(PrintNode   &n) override;
    void visit(IfNode      &n) override;
    void visit(WhileNode   &n) override;
    void visit(BinOpNode   &n) override;
    void visit(IdNode      &n) override;
    void visit(IntLitNode  &n) override;
    void visit(StrLitNode  &n) override;

private:
    std::vector<TACInstr> code_;   // accumulated instructions
    std::string           result_; // value produced by the last expression visit
    int                   tempCnt_  = 0;
    int                   labelCnt_ = 0;

    std::string freshTemp()  { return "t" + std::to_string(tempCnt_++);  }
    std::string freshLabel() { return "L" + std::to_string(labelCnt_++); }

    void emit(TACInstr i) { code_.push_back(std::move(i)); }

    // Evaluate an expression node and return the name of the variable/temp
    // that holds the result.
    std::string evalExpr(ASTNode &node);
};

#endif // MINI_ICG_H
