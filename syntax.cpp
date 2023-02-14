#include <iostream>
#include "syntax.h"
#include "exception.h"

namespace Syntax {
    std::vector <Token> TokenStream;
    int TokenPosition;
    void SetTokenStream(const std::vector <Token> &_TokenStream) {
        TokenPosition = 0;
        TokenStream = _TokenStream;
    }

    Token GetToken() {
        return TokenStream[TokenPosition];
    }

    bool NextToken() {
        TokenPosition++;
        return TokenStream[TokenPosition].type != TokenType::Eof;
    }

    bool CheckToken() {
        return TokenStream[TokenPosition].type != TokenType::Eof;
    }

    std::shared_ptr <AST::Block> ProcessBlock() {
        std::shared_ptr <AST::Block> block = std::make_shared <AST::Block> ();
        block->line_begin = GetToken().line;
        block->position_begin = GetToken().position;
        while(CheckToken() && GetToken().type != TokenType::BraceClose) {
            std::shared_ptr <AST::Statement> _statement = ProcessStatement();
            if (_statement) {
                block->statement_list.push_back(_statement);
            }
        }
        if (GetToken().type != TokenType::BraceClose) {
            throw AliasException("{ expected after block", GetToken());
        }
        block->line_begin = GetToken().line;
        block->position_begin = GetToken().position;
        return block;
    }

    std::shared_ptr <AST::Expression> ProcessExpression() {
        std::shared_ptr <AST::Expression> _expression1 = ProcessPrimary();
        if (GetToken().type == TokenType::Plus) {
            NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression();
            std::shared_ptr <AST::Addition> _addition = std::make_shared <AST::Addition> ();
            _addition->left = _expression1;
            _addition->right = _expression2;
            _addition->line_begin = _expression1->line_begin;
            _addition->position_begin = _expression1->position_begin;
            _addition->line_end= _expression2->line_end;
            _addition->position_end= _expression2->position_end;
            return _addition;
        }
        if (GetToken().type == TokenType::Equal) {
            NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression();
            std::shared_ptr <AST::Equal> _equal = std::make_shared <AST::Equal> ();
            _equal->left = _expression1;
            _equal->right = _expression2;
            _equal->line_begin = _expression1->line_begin;
            _equal->position_begin = _expression1->position_begin;
            _equal->line_end= _expression2->line_end;
            _equal->position_end= _expression2->position_end;
            return _equal;
        }
        return _expression1;
    }

    std::shared_ptr <AST::Expression> ProcessPrimary() {
        if (GetToken().type == TokenType::Identifier) {
            std::shared_ptr <AST::Identifier> _identifier = std::make_shared <AST::Identifier> ();
            _identifier->identifier = GetToken().value_string;
            _identifier->line_begin = GetToken().line;
            _identifier->position_begin = GetToken().position;
            _identifier->line_end = GetToken().line;
            _identifier->position_end = GetToken().position+GetToken().length - 1;
            NextToken();
            return _identifier;
        }
        if (GetToken().type == TokenType::Integer) {
            std::shared_ptr <AST::Integer> _integer = std::make_shared <AST::Integer> ();
            _integer->value = GetToken().value_int;
            _integer->line_begin = GetToken().line;
            _integer->position_begin = GetToken().position;
            _integer->line_end = GetToken().line;
            _integer->position_end = GetToken().position+GetToken().length - 1;
            NextToken();
            return _integer;
        }
        if (GetToken().type == TokenType::Alloc) {
            std::shared_ptr <AST::Alloc> _alloc = std::make_shared <AST::Alloc> ();
            _alloc->line_begin = GetToken().line;
            _alloc->position_begin = GetToken().position;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in alloc expression", GetToken());
            }
            NextToken();
            if (GetToken().type != TokenType::Integer) {
                throw AliasException("Integer expected in alloc expression", GetToken());
            }
            _alloc->size = GetToken().value_int;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in alloc expression", GetToken());
            }
            _alloc->line_end = GetToken().line;
            _alloc->position_end = GetToken().position;
            NextToken();
            return _alloc;
        }
        throw AliasException("Identifier expected in primary expression", GetToken());
    }

    std::shared_ptr <AST::Statement> ProcessStatement() {
        if (GetToken().type == TokenType::Semicolon) {
            NextToken();
            return nullptr;
        }
        if (GetToken().type == TokenType::BraceOpen) {
            NextToken();
            std::shared_ptr <AST::Block> block = ProcessBlock();
            NextToken();
            return block;
        }
        if (GetToken().type == TokenType::If) {
            std::shared_ptr <AST::If> _if = std::make_shared <AST::If> ();
            _if->line_begin = GetToken().line;
            _if->position_begin = GetToken().position;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in if condition", GetToken());
            }
            NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessExpression();
            if (GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in if condition", GetToken());
            }
            NextToken();
            if (GetToken().type != TokenType::BraceOpen) {
                throw AliasException("{ expected in if block", GetToken());
            }
            NextToken();
            std::shared_ptr <AST::Block> _block = ProcessBlock();
            _if->branch_list.push_back({_expression, _block});
            _if->line_end = GetToken().line;
            _if->position_end = GetToken().position;
            NextToken();
            return _if;
        }
        if (GetToken().type == TokenType::Def) {
            std::shared_ptr <AST::Definition> definition = std::make_shared <AST::Definition> ();
            definition->line_begin = GetToken().line;
            definition->position_begin = GetToken().position;
            NextToken();
            if (GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier expected in definition statement", GetToken());
            }
            definition->identifier = GetToken().value_string;
            NextToken();
            if (GetToken().type != TokenType::Integer) {
                throw AliasException("Integer expected in definition statement", GetToken());
            }
            definition->type_degree = GetToken().value_int;
            definition->line_end = GetToken().line;
            definition->position_end = GetToken().position;
            NextToken();
            return definition;
        }
        if (GetToken().type == TokenType::Assume) {
            std::shared_ptr <AST::Assumption> _assumption = std::make_shared <AST::Assumption> ();
            _assumption->line_begin = GetToken().line;
            _assumption->position_begin = GetToken().position;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in assume condition", GetToken());
            }
            NextToken();
            _assumption->condition = ProcessExpression();
            if (GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in assume condition", GetToken());
            }
            _assumption->line_end = GetToken().line;
            _assumption->position_end = GetToken().position;
            NextToken();
            return _assumption;
        }
        if (GetToken().type == TokenType::Identifier) {
            std::shared_ptr <AST::Assignment> assignment = std::make_shared <AST::Assignment> ();
            assignment->line_begin = GetToken().line;
            assignment->position_begin = GetToken().position;
            assignment->identifier = GetToken().value_string;
            NextToken();
            if (GetToken().type != TokenType::Assign) {
                throw AliasException(":= expected in assignment statement", GetToken());
            }
            NextToken();
            assignment->value = ProcessExpression();
            assignment->line_end = assignment->value->line_end;
            assignment->position_end = assignment->value->position_end;
            return assignment;
        }
        throw AliasException("Statement expected", GetToken());
    }

    std::shared_ptr <AST::Node> Process(const std::vector <Token> token_stream) {
        SetTokenStream(token_stream);
        return ProcessStatement();
    }
}
