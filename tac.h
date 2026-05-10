/*
 * tac.h  —  Three-Address Code (TAC) Intermediate Representation.
 *
 * Every TAC instruction fits one of these forms:
 *
 *   result = arg1 OP arg2       (BinaryOp)
 *   result = arg1               (Copy)
 *   result = arg1               (CopyLit  — arg1 is a literal string)
 *   PRINT arg1                  (Print)
 *   GOTO label                  (Goto)
 *   IF arg1 GOTO label          (IfTrue)
 *   IFFALSE arg1 GOTO label     (IfFalse)
 *   label:                      (Label)
 *   PARAM arg1                  (Param  — for future function calls)
 *   CALL func n                 (Call)
 *   RETURN arg1                 (Return)
 *
 * Operands are plain strings:
 *   - temporaries:  "t0", "t1", …
 *   - identifiers:  variable names from the source
 *   - literals:     the lexeme verbatim ("42", "\"hello\"")
 *   - labels:       "L0", "L1", …
 */

#ifndef MINI_TAC_H
#define MINI_TAC_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

enum class TACOp {
    BinaryOp,   // result = arg1 OP arg2
    Copy,       // result = arg1
    Print,      // PRINT arg1
    Goto,       // GOTO label
    IfTrue,     // IF arg1 GOTO label
    IfFalse,    // IFFALSE arg1 GOTO label
    Label,      // label:
    Return      // RETURN arg1
};

struct TACInstr {
    TACOp       op;
    std::string result;   // destination / label name
    std::string arg1;
    std::string arg2;     // used only for BinaryOp
    std::string binOp;    // "+", "-", "==", "<", ">"  (BinaryOp only)

    // ── factory helpers ───────────────────────────────────────────────────────

    static TACInstr binaryOp(std::string res, std::string a1,
                              std::string bop, std::string a2)
    {
        TACInstr i;
        i.op = TACOp::BinaryOp;
        i.result = std::move(res);
        i.arg1   = std::move(a1);
        i.binOp  = std::move(bop);
        i.arg2   = std::move(a2);
        return i;
    }

    static TACInstr copy(std::string res, std::string a1)
    {
        TACInstr i;
        i.op     = TACOp::Copy;
        i.result = std::move(res);
        i.arg1   = std::move(a1);
        return i;
    }

    static TACInstr print(std::string a1)
    {
        TACInstr i;
        i.op   = TACOp::Print;
        i.arg1 = std::move(a1);
        return i;
    }

    static TACInstr gotoLabel(std::string label)
    {
        TACInstr i;
        i.op     = TACOp::Goto;
        i.result = std::move(label);
        return i;
    }

    static TACInstr ifTrue(std::string cond, std::string label)
    {
        TACInstr i;
        i.op     = TACOp::IfTrue;
        i.arg1   = std::move(cond);
        i.result = std::move(label);
        return i;
    }

    static TACInstr ifFalse(std::string cond, std::string label)
    {
        TACInstr i;
        i.op     = TACOp::IfFalse;
        i.arg1   = std::move(cond);
        i.result = std::move(label);
        return i;
    }

    static TACInstr label(std::string name)
    {
        TACInstr i;
        i.op     = TACOp::Label;
        i.result = std::move(name);
        return i;
    }
};

// ── pretty-printer ────────────────────────────────────────────────────────────

inline void printTAC(const std::vector<TACInstr> &code, std::ostream &out = std::cout)
{
    int lineNum = 1;
    for (const auto &instr : code) {
        switch (instr.op) {
        case TACOp::Label:
            out << "\n" << instr.result << ":\n";
            break;

        case TACOp::BinaryOp:
            out << std::setw(4) << lineNum++ << "    "
                << instr.result << " = "
                << instr.arg1 << " " << instr.binOp << " " << instr.arg2 << "\n";
            break;

        case TACOp::Copy:
            out << std::setw(4) << lineNum++ << "    "
                << instr.result << " = " << instr.arg1 << "\n";
            break;

        case TACOp::Print:
            out << std::setw(4) << lineNum++ << "    "
                << "PRINT " << instr.arg1 << "\n";
            break;

        case TACOp::Goto:
            out << std::setw(4) << lineNum++ << "    "
                << "GOTO " << instr.result << "\n";
            break;

        case TACOp::IfTrue:
            out << std::setw(4) << lineNum++ << "    "
                << "IF " << instr.arg1 << " GOTO " << instr.result << "\n";
            break;

        case TACOp::IfFalse:
            out << std::setw(4) << lineNum++ << "    "
                << "IFFALSE " << instr.arg1 << " GOTO " << instr.result << "\n";
            break;

        case TACOp::Return:
            out << std::setw(4) << lineNum++ << "    "
                << "RETURN " << instr.arg1 << "\n";
            break;
        }
    }
}

#endif // MINI_TAC_H
