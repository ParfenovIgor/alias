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
class Definition;
class Assignment;
class Assumption;
class Expression;
class Identifier;
class Integer;
class Alloc;
class Free;
class Addition;
class Less;
class Equal;

enum class Type {
    Int,
    Ptr,
};

class Node {
public:
    virtual ~Node(){};
    virtual void Validate() = 0;
    int line_begin, position_begin, line_end, position_end;
    // virtual void Print(std::ostream &out, int ind = 0) = 0;
};

class Statement : public Node {
};

class Block : public Statement {
public:
    std::vector <std::shared_ptr <Statement>> statement_list;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class If : public Statement {
public:
    std::vector < std::pair <std::shared_ptr <Expression>, std::shared_ptr<Block>>> branch_list;
    std::shared_ptr <Block> else_body;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class While : public Statement {
public:
    std::shared_ptr <Expression> expression;
    std::shared_ptr <Block> block;
    void Validate();
};

class Definition : public Statement {
public:
    std::string identifier;
    Type type;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Assignment : public Statement {
public:
    std::string identifier;
    std::shared_ptr <Expression> value;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Movement : public Statement {
public:
    std::string identifier;
    std::shared_ptr <Expression> value;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Assumption : public Statement {
public:
    std::shared_ptr <Expression> condition;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Expression : public Node {
};

class Identifier : public Expression {
public:
    std::string identifier;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Integer : public Expression {
public:
    int value;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Alloc : public Expression {
public:
    int size;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Free : public Expression {
public:
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Addition : public Expression {
public:
    std::shared_ptr <Expression> left, right;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Less : public Expression {
public:
    std::shared_ptr <Expression> left, right;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

class Equal : public Expression {
public:
    std::shared_ptr <Expression> left, right;
    void Validate();
    // void Print(std::ostream &out, int ind = 0);
};

}

#endif // AST_H_INCLUDED
