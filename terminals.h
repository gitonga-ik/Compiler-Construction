#ifndef COMPILER_CONSTRUCTION_TERMINALS_H
#define COMPILER_CONSTRUCTION_TERMINALS_H

#include <string>

enum Terminal
{
    T_BEGIN,
    T_END,
    T_INT,
    T_STRING,
    T_WHILE,
    T_IF,
    T_ELSE,
    T_COUT,
    T_ID,
    T_INTEGER,
    T_STRING_LITERAL,

    T_ASSIGN,
    T_EQ,
    T_GT,
    T_LT,
    T_PLUS,
    T_MINUS,
    T_OP_OUT,
    T_SCOLON,
    T_LBRACE,
    T_RBRACE,
    T_LPAREN,
    T_RPAREN,
    T_EOF
};

enum NonTerminal
{
    NT_PROGRAM,
    NT_STMT_LIST,
    NT_STMT_LIST_PRIME,
    NT_STMT,
    NT_ASSIGNMENT,
    NT_PRINT_STMT,
    NT_IF_STMT,
    NT_ELSE_PART,
    NT_WHILE_STMT,
    NT_EXPR,
    NT_EXPR_PRIME,
    NT_ARITH_EXPR,
    NT_ARITH_EXPR_PRIME,
    NT_TERM,
    NT_TYPE,
    _NT_COUNT
};

struct Symbol
{
    bool isTerminal;
    union
    {
        Terminal terminal;
        NonTerminal non_terminal;
    };

    static Symbol term(Terminal terminal)
    {
        Symbol symbol;
        symbol.isTerminal = true;
        symbol.terminal = terminal;
        return symbol;
    }
    static Symbol nonterm(NonTerminal non_terminal)
    {
        Symbol symbol;
        symbol.isTerminal = false;
        symbol.non_terminal = non_terminal;
        return symbol;
    }
};


inline Symbol T(Terminal t) { return Symbol::term(t); }
inline Symbol NT(NonTerminal nt) { return Symbol::nonterm(nt); }

const char *termName(Terminal terminal);

const char *ntName(NonTerminal non_terminal);

Terminal nameToTerminal(const std::string &name);

struct Terminals {
    Terminal type;
    std::string lexeme;
    int line = 0;      
};

#endif
