#ifndef COMPILE_H_INCLUDED
#define COMPILE_H_INCLUDED

#include "ast.h"

namespace AST {
    void Compile(std::shared_ptr <AST::Node> node, std::ostream &_out);
}

#endif // COMPILE_H_INCLUDED