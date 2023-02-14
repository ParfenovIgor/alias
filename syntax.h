#ifndef SYNTAX_H_INCLUDED
#define SYNTAX_H_INCLUDED

#include <memory>
#include "token.h"
#include "ast.h"

namespace Syntax {
    void SetTokenStream(const std::vector <Token> &_TokenStream);
    Token GetToken();
    bool NextToken();
    bool CheckToken();

    std::shared_ptr <AST::Block> ProcessBlock();
    std::shared_ptr <AST::Expression> ProcessExpression();
    std::shared_ptr <AST::Expression> ProcessPrimary();
    std::shared_ptr <AST::Statement> ProcessStatement();
    std::shared_ptr <AST::Node> Process(const std::vector <Token> token_stream);
}

#endif // SYNTAX_H_INCLUDED
