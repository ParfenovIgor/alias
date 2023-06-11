#ifndef SYNTAX_H_INCLUDED
#define SYNTAX_H_INCLUDED

#include <memory>
#include "token.h"
#include "ast.h"

namespace Syntax {
    class TokenStream {
    private:
        std::vector <Token> stream;
        int pos;
    public:
        TokenStream(std::vector <Token> _stream) {
            stream = _stream;
            pos = 0;
        }

        Token GetToken() {
            return stream[pos];
        }

        void NextToken() {
            pos++;
        }
    };

    std::shared_ptr <AST::Block> ProcessProgram(TokenStream&);
    std::shared_ptr <AST::Block> ProcessBlock(TokenStream&);
    std::shared_ptr <AST::Expression> ProcessExpression(TokenStream&);
    std::shared_ptr <AST::Expression> ProcessPrimary(TokenStream&);
    std::shared_ptr <AST::Statement> ProcessStatement(TokenStream&);
    std::shared_ptr <AST::Node> Process(const std::vector <Token> token_stream);
}

#endif // SYNTAX_H_INCLUDED
