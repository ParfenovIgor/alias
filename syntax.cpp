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
        block->line_begin = GetToken().line_begin;
        block->position_begin = GetToken().position_begin;
        NextToken();
        while(CheckToken() && GetToken().type != TokenType::BraceClose) {
            std::shared_ptr <AST::Statement> _statement = ProcessStatement();
            if (_statement) {
                block->statement_list.push_back(_statement);
            }
        }
        if (GetToken().type != TokenType::BraceClose) {
            throw AliasException("} expected after block", GetToken());
        }
        block->line_end = GetToken().line_end;
        block->position_end = GetToken().position_end;
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
        if (GetToken().type == TokenType::Less) {
            NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression();
            std::shared_ptr <AST::Less> _less = std::make_shared <AST::Less> ();
            _less->left = _expression1;
            _less->right = _expression2;
            _less->line_begin = _expression1->line_begin;
            _less->position_begin = _expression1->position_begin;
            _less->line_end= _expression2->line_end;
            _less->position_end= _expression2->position_end;
            return _less;
        }
        return _expression1;
    }

    std::shared_ptr <AST::Expression> ProcessPrimary() {
        if (GetToken().type == TokenType::Dereference) {
            std::shared_ptr <AST::Dereference> _dereference = std::make_shared <AST::Dereference> ();
            _dereference->line_begin = GetToken().line_begin;
            _dereference->position_begin = GetToken().position_begin;
            NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessPrimary();
            _dereference->line_end = _expression->line_end;
            _dereference->position_end = _expression->position_end;
            _dereference->arg = _expression;
            return _dereference;
        }
        if (GetToken().type == TokenType::Identifier) {
            std::shared_ptr <AST::Identifier> _identifier = std::make_shared <AST::Identifier> ();
            _identifier->identifier = GetToken().value_string;
            _identifier->line_begin = GetToken().line_begin;
            _identifier->position_begin = GetToken().position_begin;
            _identifier->line_end = GetToken().line_end;
            _identifier->position_end = GetToken().position_end;
            NextToken();
            return _identifier;
        }
        if (GetToken().type == TokenType::Integer) {
            std::shared_ptr <AST::Integer> _integer = std::make_shared <AST::Integer> ();
            _integer->value = GetToken().value_int;
            _integer->line_begin = GetToken().line_begin;
            _integer->position_begin = GetToken().position_begin;
            _integer->line_end = GetToken().line_end;
            _integer->position_end = GetToken().position_end;
            NextToken();
            return _integer;
        }
        if (GetToken().type == TokenType::Alloc) {
            std::shared_ptr <AST::Alloc> _alloc = std::make_shared <AST::Alloc> ();
            _alloc->line_begin = GetToken().line_begin;
            _alloc->position_begin = GetToken().position_begin;
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
            _alloc->line_end = GetToken().line_end;
            _alloc->position_end = GetToken().position_end;
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
            std::shared_ptr <AST::Block> block = ProcessBlock();
            NextToken();
            return block;
        }
        if (GetToken().type == TokenType::Asm) {
            std::shared_ptr <AST::Asm> _asm = std::make_shared <AST::Asm> ();
            _asm->line_begin = GetToken().line_begin;
            _asm->position_begin = GetToken().position_begin;
            _asm->line_end = GetToken().line_end;
            _asm->position_end = GetToken().position_end;
            _asm->code = GetToken().value_string;
            NextToken();
            return _asm;
        }
        if (GetToken().type == TokenType::If) {
            std::shared_ptr <AST::If> _if = std::make_shared <AST::If> ();
            _if->line_begin = GetToken().line_begin;
            _if->position_begin = GetToken().position_begin;
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
            std::shared_ptr <AST::Block> _block = ProcessBlock();
            _if->branch_list.push_back({_expression, _block});
            _if->line_end = GetToken().line_end;
            _if->position_end = GetToken().position_end;
            NextToken();

            if (GetToken().type == TokenType::Else) {
                NextToken();
                if (GetToken().type != TokenType::BraceOpen) {
                    throw AliasException("{ expected in if block", GetToken());
                }
                std::shared_ptr <AST::Block> _block = ProcessBlock();
                _if->else_body = _block;
                _if->line_end = GetToken().line_end;
                _if->position_end = GetToken().position_end;
                NextToken();
            }

            return _if;
        }
        if (GetToken().type == TokenType::While) {
            std::shared_ptr <AST::While> _while = std::make_shared <AST::While> ();
            _while->line_begin = GetToken().line_begin;
            _while->position_begin = GetToken().position_begin;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in while condition", GetToken());
            }
            NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessExpression();
            if (GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in while condition", GetToken());
            }
            NextToken();
            if (GetToken().type != TokenType::BraceOpen) {
                throw AliasException("{ expected in while block", GetToken());
            }
            std::shared_ptr <AST::Block> _block = ProcessBlock();
            _while->expression = _expression;
            _while->block = _block;
            _while->line_end = GetToken().line_end;
            _while->position_end = GetToken().position_end;
            NextToken();

            return _while;
        }
        if (GetToken().type == TokenType::Func) {
            std::shared_ptr <AST::FunctionDefinition> function_definition = std::make_shared <AST::FunctionDefinition> ();
            function_definition->line_begin = GetToken().line_begin;
            function_definition->position_begin = GetToken().position_begin;
            NextToken();
            if (GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier exprected in function definition", GetToken());
            }
            function_definition->name = GetToken().value_string;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in function definition", GetToken());
            }
            NextToken();
            std::shared_ptr <AST::FunctionSignature> function_signature = std::make_shared <AST::FunctionSignature> ();
            while (true) {
                if (GetToken().type != TokenType::Identifier) {
                    throw AliasException("Idenfier expected in argument list", GetToken());
                }
                function_signature->identifiers.push_back(GetToken().value_string);
                NextToken();
                if (GetToken().type != TokenType::Int && GetToken().type != TokenType::Ptr) {
                    throw AliasException("Type expected in argument list", GetToken());
                }
                if (GetToken().type == TokenType::Int) {
                    function_signature->types.push_back(AST::Type::Int);
                    NextToken();
                    function_signature->size_in.push_back(0);
                    function_signature->size_out.push_back(0);
                }
                else {
                    function_signature->types.push_back(AST::Type::Ptr);
                    NextToken();
                    if (GetToken().type != TokenType::Integer) {
                        throw AliasException("Integer expected in pointer argument", GetToken());
                    }
                    function_signature->size_in.push_back(GetToken().value_int);
                    NextToken();
                    if (GetToken().type != TokenType::Integer) {
                        throw AliasException("Integer expected in pointer argument", GetToken());
                    }
                    function_signature->size_out.push_back(GetToken().value_int);
                    NextToken();
                }
                if (GetToken().type == TokenType::ParenthesisClose) {
                    NextToken();
                    break;
                }
                if (GetToken().type != TokenType::Comma) {
                    throw AliasException(", expected in argument list", GetToken());
                }
                NextToken();
            }
            function_definition->signature = function_signature;
            if (GetToken().type != TokenType::BraceOpen) {
                throw AliasException("{ expected in function block", GetToken());
            }
            std::shared_ptr <AST::Block> _block = ProcessBlock();
            function_definition->body = _block;
            function_definition->line_end = GetToken().line_end;
            function_definition->position_end = GetToken().position_end;
            NextToken();

            return function_definition;
        }
        if (GetToken().type == TokenType::Def) {
            std::shared_ptr <AST::Definition> definition = std::make_shared <AST::Definition> ();
            definition->line_begin = GetToken().line_begin;
            definition->position_begin = GetToken().position_begin;
            NextToken();
            if (GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier expected in definition statement", GetToken());
            }
            definition->identifier = GetToken().value_string;
            NextToken();
            if (GetToken().type != TokenType::Int && GetToken().type != TokenType::Ptr) {
                throw AliasException("Type expected in definition statement", GetToken());
            }
            if (GetToken().type == TokenType::Int) {
                definition->type = AST::Type::Int;
            }
            else {
                definition->type = AST::Type::Ptr;
            }
            definition->line_end = GetToken().line_end;
            definition->position_end = GetToken().position_end;
            NextToken();
            return definition;
        }
        if (GetToken().type == TokenType::Assume) {
            std::shared_ptr <AST::Assumption> _assumption = std::make_shared <AST::Assumption> ();
            _assumption->line_begin = GetToken().line_begin;
            _assumption->position_begin = GetToken().position_begin;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in assume condition", GetToken());
            }
            NextToken();
            _assumption->condition = ProcessExpression();
            if (GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in assume condition", GetToken());
            }
            _assumption->line_end = GetToken().line_end;
            _assumption->position_end = GetToken().position_end;
            NextToken();
            return _assumption;
        }
        if (GetToken().type == TokenType::Free) {
            std::shared_ptr <AST::Free> _free = std::make_shared <AST::Free> ();
            _free->line_begin = GetToken().line_begin;
            _free->position_begin = GetToken().position_begin;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in free statement", GetToken());
            }
            NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessExpression();
            _free->arg = _expression;
            if (GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in free expression", GetToken());
            }
            _free->line_end = GetToken().line_end;
            _free->position_end = GetToken().position_end;
            NextToken();
            return _free;
        }
        if (GetToken().type == TokenType::Call) {
            std::shared_ptr <AST::FunctionCall> function_call = std::make_shared <AST::FunctionCall> ();
            function_call->line_begin = GetToken().line_begin;
            function_call->position_begin = GetToken().position_begin;
            NextToken();
            if (GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier expected in function call", GetToken());
            }
            function_call->identifier = GetToken().value_string;
            NextToken();
            if (GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in function call", GetToken());
            }
            NextToken();
            while (true) {
                if (GetToken().type == TokenType::Identifier) {
                    function_call->arguments.push_back(GetToken().value_string);
                }
                else {
                    throw AliasException("Identifier expected in function call", GetToken());
                }
                NextToken();
                if (GetToken().type == TokenType::ParenthesisClose) {
                    break;
                }
                if (GetToken().type != TokenType::Comma) {
                    throw AliasException(", expectred in function call", GetToken());
                }
                NextToken();
            }
            function_call->line_end = GetToken().line_end;
            function_call->position_end = GetToken().position_end;
            NextToken();
            return function_call;
        }
        if (GetToken().type == TokenType::Identifier) {
            int line_begin = GetToken().line_begin;
            int position_begin = GetToken().position_begin;
            std::string identifier = GetToken().value_string;
            NextToken();
            if (GetToken().type != TokenType::Assign && GetToken().type != TokenType::Move) {
                throw AliasException(":= or <- expected in assignment or movement statement", GetToken());
            }

            if (GetToken().type == TokenType::Assign) {
                std::shared_ptr <AST::Assignment> assignment = std::make_shared <AST::Assignment> ();
                assignment->line_begin = line_begin;
                assignment->position_begin = position_begin;
                assignment->identifier = identifier;
                NextToken();
                assignment->value = ProcessExpression();
                assignment->line_end = assignment->value->line_end;
                assignment->position_end = assignment->value->position_end;
                return assignment;
            }

            if (GetToken().type == TokenType::Move) {
                std::shared_ptr <AST::Movement> movement = std::make_shared <AST::Movement> ();
                movement->line_begin = line_begin;
                movement->position_begin = position_begin;
                movement->identifier = identifier;
                NextToken();
                movement->value = ProcessExpression();
                movement->line_end = movement->value->line_end;
                movement->position_end = movement->value->position_end;
                return movement;
            }
        }
        throw AliasException("Statement expected", GetToken());
    }

    std::shared_ptr <AST::Node> Process(const std::vector <Token> token_stream) {
        SetTokenStream(token_stream);
        return ProcessStatement();
    }
}
