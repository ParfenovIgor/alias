#ifndef VALIDATOR_H_INCLUDED
#define VALIDATOR_H_INCLUDED

#include "ast.h"

namespace AST {
    struct State {
        std::vector <std::pair <int, int> > heap; // {index, size}
    };

    enum class Type {
        Int,
        Ptr,
    };

    bool operator < (State &a, State &b);

    int getIndex(std::string id);
    Type getType(std::string id);
    void simplifyStates();
    void printStates();
    std::vector <std::pair <std::string, int> > variable_stack; // {name, index}
    std::vector <Type> variable_type;
    std::vector <int> heap_size;
    int variable_index = 0;
    int heap_index = 0;

    std::vector <State> states;

    int spaces = 0;
}

#endif // VALIDATOR_H_INCLUDED
