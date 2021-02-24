#include "expression.hpp"

template <class ...TArgs>
AST_FunctionCall::AST_FunctionCall(std::string _functionName, TArgs... _args):
    functionName(_functionName)
{
    args = _args
    parity = args.size();
}

void AST_FunctionCall::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_FunctionCall::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_FunctionCall::~AST_FunctionCall() {
    // delete name;
}

AST_BinOp::AST_BinOp(AST_BinOp::Type _type, AST* _left, AST* _right):
    type(_type),
    left(_left),
    right(_right)
{}

void AST_BinOp::generateFrames(Frame* _frame){
    frame = _frame;
    left->generateFrames(_frame);
    right->generateFrames(_frame);
}

void AST_BinOp::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("Not Implemented Yet.\n");
}
