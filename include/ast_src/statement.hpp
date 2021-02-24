#pragma once

#include "ast.hpp"

/*
    Function return statement.
    The returned expression must have the type of the function.
    Void functions have no expression.
    The main function returns 0 by default.
*/
class AST_Return
    : public AST
{
private:
    AST* expr;

public:
    AST_Return(AST* _expr = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_Return();
};

/*
    Represents both standalone if-statements and combined if-else statements
    Does not yet support if-elseif-else style statements
    
    Actually it does
    if(){
    }
    else if(){
    }
    else{
    }

    can be re-written as

    if(){
    }
    else{
        if(){
        }
        else{
        }
    }

    Except since the if-else is a single statement, the expression doesn't need curly brakets
*/
class AST_IfStmt
    : public AST
{
private:
    AST* cond;
    AST* then;
    AST* other;

public:
    AST_IfStmt(AST* _cond, AST* _then, AST* _other = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_IfStmt();
}
