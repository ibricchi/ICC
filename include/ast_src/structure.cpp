#include "structure.hpp"
#include "expression.hpp"

AST_Sequence::AST_Sequence(AST* _first, AST* _second) :
    first(_first),
    second(_second)
{}

void AST_Sequence::generateFrames(Frame* _frame){
    frame = _frame;
    first->generateFrames(_frame);
    second->generateFrames(_frame);
}

void AST_Sequence::compile(std::ostream &assemblyOut) {
    first->compile(assemblyOut);
    second->compile(assemblyOut);
}

AST_Sequence::~AST_Sequence(){
    delete first;
    delete second;
}

AST_FunDeclaration::AST_FunDeclaration(std::string _type, std::string* _name, AST* _body, std::vector<std::pair<std::string,std::string>>* _params) :
    type(_type),
    name(*_name),
    body(_body),
    params(_params)

{
    parity = 0;
    if(_params != nullptr) parity = _params->size();
}

void AST_FunDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    // we don't need to generate a new frame here since the block statement that will be the body
    // will handle generating the new frame
    if (body != nullptr) {
        body->generateFrames(_frame);
        body->frame->isFun = true;
        // declare parameters as variables in the frame
        if(params != nullptr)
            for(std::pair<std::string,std::string> param: *params){
                body->frame->addVariable(param.second, getTypeByteSize(param.first));;
            }
    } 
}

void AST_FunDeclaration::compile(std::ostream &assemblyOut) {
    assemblyOut << std::endl << "# start function declaration for "<< name << std::endl;
    if (body != nullptr) {
        // function header
        assemblyOut << ".align  2" << std::endl;
        assemblyOut << ".global " << name << std::endl;
        assemblyOut << ".set	nomips16" << std::endl;
        assemblyOut << ".set	nomicromips" << std::endl;
        assemblyOut << ".ent    " << name << std::endl;
        assemblyOut << ".type   " << name << ", @function" << std::endl;

        // create label
        assemblyOut << name << ":" << std::endl;

        // function header 2
        assemblyOut << ".frame	$fp, " << body->frame->getStoreSize() << " , $31" << std::endl;
        assemblyOut << ".mask	0x40000000,-4" << std::endl;
        assemblyOut << ".fmask	0x00000000,0" << std::endl;
        assemblyOut << ".set	noreorder" << std::endl;
        assemblyOut << ".set	nomacro" << std::endl;

        // increase size of current frame by required ammount for storing previous state data
        // currently storing only $31, and $fp
        assemblyOut << "addiu $sp, $sp, -" << body->frame->getStoreSize() << std::endl;
        assemblyOut << "sw $31, 8($sp)" << std::endl;
        assemblyOut << "sw $fp, 12($sp)" << std::endl;
        assemblyOut << "move $fp, $sp" << std::endl;

        // move stack pointer down to allocate space for temporary variables in frame
        assemblyOut << "addiu $sp, $sp, -" << body->frame->getVarSize() << std::endl;

        // copy over arguments from call
        if(params != nullptr){
            for(int i = 0, arg_i = params->size() - 1; i < params->size(); i++, arg_i--){
                // load from register
                if(arg_i < 4){
                    // If I didn't do it like this at runtime the strings got randomly truncated
                    std::string reg = std::string("$a") + std::to_string(arg_i);
                    std::cerr << "Passing argument " << reg << std::endl;
                    regToVar(assemblyOut, body->frame, reg, params->at(i).second);
                }
                // load from memory
                else{
                    throw std::runtime_error("Not implemented yet");
                }
            }
        }

        // body
        body->compile(assemblyOut);

        // move fp back to start of frame and re-instate previous frame
        assemblyOut << "move $sp, $fp" << std::endl;
        assemblyOut << "lw $31, 8($sp)" << std::endl;
        assemblyOut << "lw $fp, 12($sp)" << std::endl;
        assemblyOut << "addiu $sp, $sp, " << body->frame->getStoreSize() << std::endl;
        
        // jump back to wherever function was called from (this is only in place in case of void functions)
        // normally return statement will handle jumping
        assemblyOut << "jr $31" << std::endl;
        assemblyOut << "nop" << std::endl;

        // function footer
        assemblyOut << ".set	macro" << std::endl;
        assemblyOut << ".set	reorder" << std::endl;
        assemblyOut << ".end    " << name << std::endl;
        assemblyOut << ".size	" << name << ", .-" << name << std::endl;
    }
    assemblyOut << "# end function declaration for " << name << std::endl << std::endl;
}

AST_FunDeclaration::~AST_FunDeclaration() {
    if (body != nullptr) {
        delete body;
    }
    if (params != nullptr){
        delete params;
    }
}

AST_VarDeclaration::AST_VarDeclaration(std::string _type, std::string* _name, AST* _expr) :
    type(_type),
    name(*_name),
    expr(_expr)
{}

void AST_VarDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    if(expr != nullptr)
        expr->generateFrames(_frame);
    
    _frame->addVariable(name, 4);
}

void AST_VarDeclaration::compile(std::ostream &assemblyOut) {
    if (expr != nullptr) {
        assemblyOut << std::endl << "#start var dec with definition " << name << std::endl;

        expr->compile(assemblyOut);

        // load top of stack into register t0
        assemblyOut << "lw $t0, 8($sp)" << std::endl;
        assemblyOut << "addiu $sp, $sp, 8" << std::endl;

        regToVar(assemblyOut, frame, "$t0", name);
        
        assemblyOut << "#end var dec with definition " << name << std::endl << std::endl;
    }
}

AST_VarDeclaration::~AST_VarDeclaration() {
    if(expr != nullptr)
        delete expr; 
}
