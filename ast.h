#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

#include <vector>
#include <string>
#include <set>
#include <memory>

namespace AST {

class Node;
class Statement;
class Block;
class Asm;
class If;
class While;
class FunctionDefinition;
class Prototype;
class Definition;
class Assignment;
class Movement;
class MovementString;
class Assumption;
class Expression;
class Identifier;
class Integer;
class Alloc;
class Free;
class FunctionCall;
class Dereference;
class BinaryOperation;
class Addition;
class Subtraction;
class Multiplication;
class Division;
class Less;
class Equal;

enum class Type {
    Int,
    Ptr,
};

class FunctionSignature {
public:
    std::vector <std::string> identifiers;
    std::vector <Type> types;
    std::vector <std::shared_ptr <Expression>> size_in, size_out;
    std::vector <bool> is_const;
};

class FunctionSignatureEvaluated {
public:
    std::vector <std::string> identifiers;
    std::vector <Type> types;
    std::vector <int> size_in, size_out;
    std::vector <bool> is_const;
};

struct State {
    std::vector <std::pair <int, int>> heap;
};

struct VLContext {
    std::vector <std::string> variable_stack;
    std::vector <Type> variable_type_stack;
    std::vector <bool> variable_is_const_stack;
    std::vector <std::string> function_stack;
    std::vector <std::shared_ptr <FunctionSignature>> function_signature_stack;
    std::vector <FunctionDefinition*> function_pointer_stack;
    std::vector <std::set <FunctionSignatureEvaluated>> function_signature_validated;
    std::vector <int> packet_size;
    std::set <State> states;
    std::vector <std::pair <std::string, int>> metavariable_stack;
};

struct CPContext {
    std::vector <std::string> variable_stack;
    std::vector <Type> variable_stack_type;
    std::vector <std::string> variable_arguments;
    std::vector <Type> variable_arguments_type;
    std::vector <std::pair <std::string, int>> function_stack;
    int function_index = 0;
    int branch_index = 0;
};

class Node {
public:
    virtual ~Node(){};
    virtual void Validate(VLContext &context) = 0;
    virtual void Compile(std::ostream &out, CPContext &context) = 0;
    int line_begin, position_begin, line_end, position_end;
    std::string filename;
};

class Statement : public Node {
};

class Block : public Statement {
public:
    std::vector <std::shared_ptr <Statement>> statement_list;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Asm : public Statement {
public:
    std::string code;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class If : public Statement {
public:
    std::vector <std::pair <std::shared_ptr <Expression>, std::shared_ptr<Block>>> branch_list;
    std::shared_ptr <Block> else_body;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class While : public Statement {
public:
    std::shared_ptr <Expression> expression;
    std::shared_ptr <Block> block;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class FunctionDefinition : public Statement {
public:
    std::string name;
    std::vector <std::string> metavariables;
    std::shared_ptr <FunctionSignature> signature;
    std::shared_ptr <Block> body;
    bool external;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Prototype : public Statement {
public:
    std::string name;
    std::vector <std::string> metavariables;
    std::shared_ptr <FunctionSignature> signature;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Definition : public Statement {
public:
    std::string identifier;
    Type type;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Assignment : public Statement {
public:
    std::string identifier;
    std::shared_ptr <Expression> value;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Movement : public Statement {
public:
    std::string identifier;
    std::shared_ptr <Expression> value;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class MovementString : public Statement {
public:
    std::string identifier;
    std::string value;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Assumption : public Statement {
public:
    std::string identifier;
    std::shared_ptr <Expression> left, right;
    std::shared_ptr <Statement> statement;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Expression : public Node {
};

class Identifier : public Expression {
public:
    std::string identifier;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Integer : public Expression {
public:
    int value;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Alloc : public Expression {
public:
    std::shared_ptr <Expression> expression;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Free : public Statement {
public:
    std::shared_ptr <Expression> arg;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class FunctionCall : public Statement {
public:
    std::string identifier;
    std::vector <std::pair <std::string, std::shared_ptr <Expression>>> metavariables;
    std::vector <std::string> arguments;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Dereference : public Expression {
public:
    std::shared_ptr <Expression> arg;
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class BinaryOperation : public Expression {
public:
    std::shared_ptr <Expression> left, right;
    virtual ~BinaryOperation() {}
};

class Addition : public BinaryOperation {
public:
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Subtraction : public BinaryOperation {
public:
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Multiplication : public BinaryOperation {
public:
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Division : public BinaryOperation {
public:
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Less : public BinaryOperation {
public:
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

class Equal : public BinaryOperation {
public:
    void Validate(VLContext &context);
    void Compile(std::ostream &out, CPContext &context);
};

}

#endif // AST_H_INCLUDED
