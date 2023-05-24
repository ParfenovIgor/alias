#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <string>
#include <vector>
#include "token.h"

namespace Lexer {
    std::vector <Token> Process(std::string str, std::string filename);
}

#endif // LEXER_H_INCLUDED
