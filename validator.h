#ifndef VALIDATOR_H_INCLUDED
#define VALIDATOR_H_INCLUDED

#include <set>
#include "ast.h"

namespace AST {
    void Validate(std::shared_ptr <Node> node);
    void PrintStatesLog();
}

#endif // VALIDATOR_H_INCLUDED
