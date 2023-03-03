#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

#include <vector>
#include <string>
#include <memory>

namespace AST {
class Node;
class Statement;
class Block;
class If;
class While;
class FunctionDefinition;
class Definition;
class Assignment;
class Assumption;
class Expression;
class Identifier;
class Integer;
class Alloc;
class Free;
class Dereference;
class Addition;
class Less;
class Equal;

enum class Type {
    Int,
    Ptr,
};

class FunctionSignature {
public:
    std::vector < std::string > identifiers;
    std::vector <Type> types;
    std::vector <int> sizes;
};

class Node {
public:
    virtual ~Node(){};
    virtual void Validate() = 0;
    int line_begin, position_begin, line_end, position_end;
};

class Statement : public Node {
};

class Block : public Statement {
public:
    std::vector <std::shared_ptr <Statement>> statement_list;
    void Validate();
};

class If : public Statement {
public:
    std::vector < std::pair <std::shared_ptr <Expression>, std::shared_ptr<Block>>> branch_list;
    std::shared_ptr <Block> else_body;
    void Validate();
};

class While : public Statement {
public:
    std::shared_ptr <Expression> expression;
    std::shared_ptr <Block> block;
    void Validate();
};

class FunctionDefinition : public Statement {
public:
    std::string name;
    std::shared_ptr <FunctionSignature> signature;
    std::shared_ptr <Block> body;
    void Validate();
};

class Definition : public Statement {
public:
    std::string identifier;
    Type type;
    void Validate();
};

class Assignment : public Statement {
public:
    std::string identifier;
    std::shared_ptr <Expression> value;
    void Validate();
};

class Movement : public Statement {
public:
    std::string identifier;
    std::shared_ptr <Expression> value;
    void Validate();
};

class Assumption : public Statement {
public:
    std::shared_ptr <Expression> condition;
    void Validate();
};

class Expression : public Node {
};

class Identifier : public Expression {
public:
    std::string identifier;
    void Validate();
};

class Integer : public Expression {
public:
    int value;
    void Validate();
};

class Alloc : public Expression {
public:
    int size;
    void Validate();
};

class Free : public Statement {
public:
    std::shared_ptr <Expression> arg;
    void Validate();
};

class FunctionCall : public Statement {
public:
    std::string identifier;
    std::vector < std::string > arguments;
    void Validate();
};

class Dereference : public Expression {
public:
    std::shared_ptr <Expression> arg;
    void Validate();
};

class Addition : public Expression {
public:
    std::shared_ptr <Expression> left, right;
    void Validate();
};

class Less : public Expression {
public:
    std::shared_ptr <Expression> left, right;
    void Validate();
};

class Equal : public Expression {
public:
    std::shared_ptr <Expression> left, right;
    void Validate();
};

}

#endif // AST_H_INCLUDED
