#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED

#include <string>
#include "ast.h"

std::shared_ptr <AST::Node> Parse(std::string filename);
int Process();

#endif // PROCESS_H_INCLUDED