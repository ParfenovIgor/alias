#ifndef VALIDATOR_H_INCLUDED
#define VALIDATOR_H_INCLUDED

#include "ast.h"

namespace AST {
    struct State {
        std::vector < std::pair <int, int> > heap; // {index, size}
    };

    bool operator == (const State &a, const State &b);

    bool operator < (const State &a, const State &b);

    int getIndex(std::string id);
    Type getType(std::string id);
    // void simplifyStates();
    void printStatesLog();

}

#endif // VALIDATOR_H_INCLUDED
