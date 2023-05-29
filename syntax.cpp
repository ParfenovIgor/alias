#include <iostream>
#include <fstream>
#include "syntax.h"
#include "exception.h"
#include "process.h"

namespace Syntax {
    std::shared_ptr <AST::Block> ProcessProgram(TokenStream &ts) {
        std::shared_ptr <AST::Block> block = std::make_shared <AST::Block> ();
        block->line_begin = ts.GetToken().line_begin;
        block->position_begin = ts.GetToken().position_begin;
        while(ts.GetToken().type != TokenType::Eof && ts.GetToken().type != TokenType::BraceClose) {
            if (ts.GetToken().type == TokenType::Include) {
                std::string filename = ts.GetToken().value_string;
                std::ifstream fin(filename);
                if (!fin) {
                    throw AliasException("Could not open file " + filename, ts.GetToken());
                }
                std::shared_ptr <AST::Node> node = Parse(filename);
                std::shared_ptr <AST::Block> inc_block = std::dynamic_pointer_cast <AST::Block> (node);
                for (auto inc_statement : inc_block->statement_list) {
                    block->statement_list.push_back(inc_statement);
                }
                ts.NextToken();
                continue;
            }
            std::shared_ptr <AST::Statement> _statement = ProcessStatement(ts);
            if (_statement) {
                block->statement_list.push_back(_statement);
            }
        }
        block->line_end = ts.GetToken().line_end;
        block->position_end = ts.GetToken().position_end;
        block->filename = ts.GetToken().filename;
        return block;
    }

    std::shared_ptr <AST::Block> ProcessBlock(TokenStream &ts) {
        std::shared_ptr <AST::Block> block = std::make_shared <AST::Block> ();
        block->line_begin = ts.GetToken().line_begin;
        block->position_begin = ts.GetToken().position_begin;
        ts.NextToken();
        while(ts.GetToken().type != TokenType::Eof && ts.GetToken().type != TokenType::BraceClose) {
            if (ts.GetToken().type == TokenType::Include) {
                std::string filename = ts.GetToken().value_string;
                std::ifstream fin(filename);
                if (!fin) {
                    throw AliasException("Could not open file " + filename, ts.GetToken());
                }
                std::shared_ptr <AST::Node> node = Parse(filename);
                std::shared_ptr <AST::Block> inc_block = std::dynamic_pointer_cast <AST::Block> (node);
                for (auto inc_statement : inc_block->statement_list) {
                    block->statement_list.push_back(inc_statement);
                }
                ts.NextToken();
                continue;
            }
            std::shared_ptr <AST::Statement> _statement = ProcessStatement(ts);
            if (_statement) {
                block->statement_list.push_back(_statement);
            }
        }
        if (ts.GetToken().type != TokenType::BraceClose) {
            throw AliasException("} expected after block", ts.GetToken());
        }
        block->line_end = ts.GetToken().line_end;
        block->position_end = ts.GetToken().position_end;
        block->filename = ts.GetToken().filename;
        return block;
    }

    std::shared_ptr <AST::Expression> ProcessExpression(TokenStream &ts) {
        std::shared_ptr <AST::Expression> _expression1 = ProcessPrimary(ts);
        if (ts.GetToken().type == TokenType::Plus) {
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression(ts);
            std::shared_ptr <AST::Addition> _addition = std::make_shared <AST::Addition> ();
            _addition->left = _expression1;
            _addition->right = _expression2;
            _addition->line_begin = _expression1->line_begin;
            _addition->position_begin = _expression1->position_begin;
            _addition->line_end= _expression2->line_end;
            _addition->position_end= _expression2->position_end;
            _addition->filename = _expression1->filename;
            return _addition;
        }
        if (ts.GetToken().type == TokenType::Minus) {
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression(ts);
            std::shared_ptr <AST::Subtraction> _subtraction = std::make_shared <AST::Subtraction> ();
            _subtraction->left = _expression1;
            _subtraction->right = _expression2;
            _subtraction->line_begin = _expression1->line_begin;
            _subtraction->position_begin = _expression1->position_begin;
            _subtraction->line_end= _expression2->line_end;
            _subtraction->position_end= _expression2->position_end;
            _subtraction->filename = _expression1->filename;
            return _subtraction;
        }
        if (ts.GetToken().type == TokenType::Mult) {
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression(ts);
            std::shared_ptr <AST::Multiplication> _multiplication = std::make_shared <AST::Multiplication> ();
            _multiplication->left = _expression1;
            _multiplication->right = _expression2;
            _multiplication->line_begin = _expression1->line_begin;
            _multiplication->position_begin = _expression1->position_begin;
            _multiplication->line_end= _expression2->line_end;
            _multiplication->position_end= _expression2->position_end;
            _multiplication->filename = _expression1->filename;
            return _multiplication;
        }
        if (ts.GetToken().type == TokenType::Less) {
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression(ts);
            std::shared_ptr <AST::Less> _less = std::make_shared <AST::Less> ();
            _less->left = _expression1;
            _less->right = _expression2;
            _less->line_begin = _expression1->line_begin;
            _less->position_begin = _expression1->position_begin;
            _less->line_end= _expression2->line_end;
            _less->position_end= _expression2->position_end;
            _less->filename = _expression1->filename;
            return _less;
        }
        if (ts.GetToken().type == TokenType::Equal) {
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression2 = ProcessExpression(ts);
            std::shared_ptr <AST::Equal> _equal = std::make_shared <AST::Equal> ();
            _equal->left = _expression1;
            _equal->right = _expression2;
            _equal->line_begin = _expression1->line_begin;
            _equal->position_begin = _expression1->position_begin;
            _equal->line_end= _expression2->line_end;
            _equal->position_end= _expression2->position_end;
            _equal->filename = _expression1->filename;
            return _equal;
        }
        return _expression1;
    }

    std::shared_ptr <AST::Expression> ProcessPrimary(TokenStream &ts) {
        if (ts.GetToken().type == TokenType::Dereference) {
            std::shared_ptr <AST::Dereference> _dereference = std::make_shared <AST::Dereference> ();
            _dereference->line_begin = ts.GetToken().line_begin;
            _dereference->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessPrimary(ts);
            _dereference->line_end = _expression->line_end;
            _dereference->position_end = _expression->position_end;
            _dereference->filename = _expression->filename;
            _dereference->arg = _expression;
            return _dereference;
        }
        if (ts.GetToken().type == TokenType::Identifier) {
            std::shared_ptr <AST::Identifier> _identifier = std::make_shared <AST::Identifier> ();
            _identifier->identifier = ts.GetToken().value_string;
            _identifier->line_begin = ts.GetToken().line_begin;
            _identifier->position_begin = ts.GetToken().position_begin;
            _identifier->line_end = ts.GetToken().line_end;
            _identifier->position_end = ts.GetToken().position_end;
            _identifier->filename = ts.GetToken().filename;
            ts.NextToken();
            return _identifier;
        }
        if (ts.GetToken().type == TokenType::Integer) {
            std::shared_ptr <AST::Integer> _integer = std::make_shared <AST::Integer> ();
            _integer->value = ts.GetToken().value_int;
            _integer->line_begin = ts.GetToken().line_begin;
            _integer->position_begin = ts.GetToken().position_begin;
            _integer->line_end = ts.GetToken().line_end;
            _integer->position_end = ts.GetToken().position_end;
            _integer->filename = ts.GetToken().filename;
            ts.NextToken();
            return _integer;
        }
        if (ts.GetToken().type == TokenType::Alloc) {
            std::shared_ptr <AST::Alloc> _alloc = std::make_shared <AST::Alloc> ();
            _alloc->line_begin = ts.GetToken().line_begin;
            _alloc->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in alloc expression", ts.GetToken());
            }
            ts.NextToken();
            _alloc->expression = ProcessExpression(ts);
            if (ts.GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in alloc expression", ts.GetToken());
            }
            _alloc->line_end = ts.GetToken().line_end;
            _alloc->position_end = ts.GetToken().position_end;
            _alloc->filename = ts.GetToken().filename;
            ts.NextToken();
            return _alloc;
        }
        throw AliasException("Identifier expected in primary expression", ts.GetToken());
    }

    std::shared_ptr <AST::Statement> ProcessStatement(TokenStream &ts) {
        if (ts.GetToken().type == TokenType::Semicolon) {
            ts.NextToken();
            return nullptr;
        }
        if (ts.GetToken().type == TokenType::BraceOpen) {
            std::shared_ptr <AST::Block> block = ProcessBlock(ts);
            ts.NextToken();
            return block;
        }
        if (ts.GetToken().type == TokenType::Asm) {
            std::shared_ptr <AST::Asm> _asm = std::make_shared <AST::Asm> ();
            _asm->line_begin = ts.GetToken().line_begin;
            _asm->position_begin = ts.GetToken().position_begin;
            _asm->line_end = ts.GetToken().line_end;
            _asm->position_end = ts.GetToken().position_end;
            _asm->code = ts.GetToken().value_string;
            _asm->filename = ts.GetToken().filename;
            ts.NextToken();
            return _asm;
        }
        if (ts.GetToken().type == TokenType::If) {
            std::shared_ptr <AST::If> _if = std::make_shared <AST::If> ();
            _if->line_begin = ts.GetToken().line_begin;
            _if->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in if condition", ts.GetToken());
            }
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessExpression(ts);
            if (ts.GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in if condition", ts.GetToken());
            }
            ts.NextToken();
            if (ts.GetToken().type != TokenType::BraceOpen) {
                throw AliasException("{ expected in if block", ts.GetToken());
            }
            std::shared_ptr <AST::Block> _block = ProcessBlock(ts);
            _if->branch_list.push_back({_expression, _block});
            _if->line_end = ts.GetToken().line_end;
            _if->position_end = ts.GetToken().position_end;
            ts.NextToken();

            if (ts.GetToken().type == TokenType::Else) {
                ts.NextToken();
                if (ts.GetToken().type != TokenType::BraceOpen) {
                    throw AliasException("{ expected in if block", ts.GetToken());
                }
                std::shared_ptr <AST::Block> _block = ProcessBlock(ts);
                _if->else_body = _block;
                _if->line_end = ts.GetToken().line_end;
                _if->position_end = ts.GetToken().position_end;
                ts.NextToken();
            }

            _if->filename = ts.GetToken().filename;
            return _if;
        }
        if (ts.GetToken().type == TokenType::While) {
            std::shared_ptr <AST::While> _while = std::make_shared <AST::While> ();
            _while->line_begin = ts.GetToken().line_begin;
            _while->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in while condition", ts.GetToken());
            }
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessExpression(ts);
            if (ts.GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in while condition", ts.GetToken());
            }
            ts.NextToken();
            if (ts.GetToken().type != TokenType::BraceOpen) {
                throw AliasException("{ expected in while block", ts.GetToken());
            }
            std::shared_ptr <AST::Block> _block = ProcessBlock(ts);
            _while->expression = _expression;
            _while->block = _block;
            _while->line_end = ts.GetToken().line_end;
            _while->position_end = ts.GetToken().position_end;
            _while->filename = ts.GetToken().filename;
            ts.NextToken();

            return _while;
        }
        if (ts.GetToken().type == TokenType::Func) {
            std::shared_ptr <AST::FunctionDefinition> function_definition = std::make_shared <AST::FunctionDefinition> ();
            function_definition->line_begin = ts.GetToken().line_begin;
            function_definition->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type == TokenType::Caret) {
                function_definition->external = true;
                ts.NextToken();
            }
            if (ts.GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier exprected in function definition", ts.GetToken());
            }
            function_definition->name = ts.GetToken().value_string;
            ts.NextToken();
            if (ts.GetToken().type == TokenType::BracketOpen) {
                ts.NextToken();
                while (true) {
                    if (ts.GetToken().type == TokenType::BracketClose) {
                        ts.NextToken();
                        break;
                    }
                    if (ts.GetToken().type != TokenType::Identifier) {
                        throw AliasException("Idenfier expected in metavariable list", ts.GetToken());
                    }
                    function_definition->metavariables.push_back(ts.GetToken().value_string);
                    ts.NextToken();
                    if (ts.GetToken().type == TokenType::BracketClose) {
                        ts.NextToken();
                        break;
                    }
                    if (ts.GetToken().type != TokenType::Comma) {
                        throw AliasException(", expected in metavariables list", ts.GetToken());
                    }
                    ts.NextToken();
                }
            }
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in function definition", ts.GetToken());
            }
            ts.NextToken();
            std::shared_ptr <AST::FunctionSignature> function_signature = std::make_shared <AST::FunctionSignature> ();
            while (true) {
                if (ts.GetToken().type == TokenType::ParenthesisClose) {
                    ts.NextToken();
                    break;
                }
                if (ts.GetToken().type != TokenType::Identifier) {
                    throw AliasException("Idenfier expected in argument list", ts.GetToken());
                }
                function_signature->identifiers.push_back(ts.GetToken().value_string);
                ts.NextToken();
                if (ts.GetToken().type != TokenType::Int && ts.GetToken().type != TokenType::Ptr) {
                    throw AliasException("Type expected in argument list", ts.GetToken());
                }
                if (ts.GetToken().type == TokenType::Int) {
                    function_signature->types.push_back(AST::Type::Int);
                    ts.NextToken();
                    function_signature->size_in.push_back(0);
                    function_signature->size_out.push_back(0);
                }
                else {
                    function_signature->types.push_back(AST::Type::Ptr);
                    ts.NextToken();
                    auto _expression1 = ProcessExpression(ts);
                    auto _expression2 = ProcessExpression(ts);
                    function_signature->size_in.push_back(_expression1);
                    function_signature->size_out.push_back(_expression2);
                }
                if (ts.GetToken().type == TokenType::ParenthesisClose) {
                    ts.NextToken();
                    break;
                }
                if (ts.GetToken().type != TokenType::Comma) {
                    throw AliasException(", expected in argument list", ts.GetToken());
                }
                ts.NextToken();
            }
            function_definition->signature = function_signature;
            if (ts.GetToken().type != TokenType::BraceOpen) {
                throw AliasException("{ expected in function block", ts.GetToken());
            }
            std::shared_ptr <AST::Block> _block = ProcessBlock(ts);
            function_definition->body = _block;
            function_definition->line_end = ts.GetToken().line_end;
            function_definition->position_end = ts.GetToken().position_end;
            function_definition->filename = ts.GetToken().filename;
            ts.NextToken();

            return function_definition;
        }
        if (ts.GetToken().type == TokenType::Proto) {
            std::shared_ptr <AST::Prototype> prototype = std::make_shared <AST::Prototype> ();
            prototype->line_begin = ts.GetToken().line_begin;
            prototype->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier exprected in function prototype", ts.GetToken());
            }
            prototype->name = ts.GetToken().value_string;
            ts.NextToken();
            if (ts.GetToken().type == TokenType::BracketOpen) {
                ts.NextToken();
                while (true) {
                    if (ts.GetToken().type == TokenType::BracketClose) {
                        ts.NextToken();
                        break;
                    }
                    if (ts.GetToken().type != TokenType::Identifier) {
                        throw AliasException("Idenfier expected in metavariable list", ts.GetToken());
                    }
                    prototype->metavariables.push_back(ts.GetToken().value_string);
                    ts.NextToken();
                    if (ts.GetToken().type == TokenType::BracketClose) {
                        ts.NextToken();
                        break;
                    }
                    if (ts.GetToken().type != TokenType::Comma) {
                        throw AliasException(", expected in metavariables list", ts.GetToken());
                    }
                    ts.NextToken();
                }
            }
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in function prototype", ts.GetToken());
            }
            ts.NextToken();
            std::shared_ptr <AST::FunctionSignature> function_signature = std::make_shared <AST::FunctionSignature> ();
            while (true) {
                if (ts.GetToken().type == TokenType::ParenthesisClose) {
                    break;
                }
                if (ts.GetToken().type != TokenType::Identifier) {
                    throw AliasException("Idenfier expected in argument list", ts.GetToken());
                }
                function_signature->identifiers.push_back(ts.GetToken().value_string);
                ts.NextToken();
                if (ts.GetToken().type != TokenType::Int && ts.GetToken().type != TokenType::Ptr) {
                    throw AliasException("Type expected in argument list", ts.GetToken());
                }
                if (ts.GetToken().type == TokenType::Int) {
                    function_signature->types.push_back(AST::Type::Int);
                    ts.NextToken();
                    function_signature->size_in.push_back(0);
                    function_signature->size_out.push_back(0);
                }
                else {
                    function_signature->types.push_back(AST::Type::Ptr);
                    ts.NextToken();
                    auto _expression1 = ProcessExpression(ts);
                    auto _expression2 = ProcessExpression(ts);
                    function_signature->size_in.push_back(_expression1);
                    function_signature->size_out.push_back(_expression2);
                }
                if (ts.GetToken().type == TokenType::ParenthesisClose) {
                    break;
                }
                if (ts.GetToken().type != TokenType::Comma) {
                    throw AliasException(", expected in argument list", ts.GetToken());
                }
                ts.NextToken();
            }
            prototype->signature = function_signature;                    
            prototype->line_end = ts.GetToken().line_end;
            prototype->position_end = ts.GetToken().position_end;
            prototype->filename = ts.GetToken().filename;
            ts.NextToken();

            return prototype;
        }
        if (ts.GetToken().type == TokenType::Def) {
            std::shared_ptr <AST::Definition> definition = std::make_shared <AST::Definition> ();
            definition->line_begin = ts.GetToken().line_begin;
            definition->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier expected in definition statement", ts.GetToken());
            }
            definition->identifier = ts.GetToken().value_string;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::Int && ts.GetToken().type != TokenType::Ptr) {
                throw AliasException("Type expected in definition statement", ts.GetToken());
            }
            if (ts.GetToken().type == TokenType::Int) {
                definition->type = AST::Type::Int;
            }
            else {
                definition->type = AST::Type::Ptr;
            }
            definition->line_end = ts.GetToken().line_end;
            definition->position_end = ts.GetToken().position_end;
            definition->filename = ts.GetToken().filename;
            ts.NextToken();
            return definition;
        }
        if (ts.GetToken().type == TokenType::Assume) {
            std::shared_ptr <AST::Assumption> _assumption = std::make_shared <AST::Assumption> ();
            _assumption->line_begin = ts.GetToken().line_begin;
            _assumption->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in assume condition", ts.GetToken());
            }
            ts.NextToken();
            if (ts.GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier expected in assume condition", ts.GetToken());
            }
            _assumption->identifier = ts.GetToken().value_string;
            ts.NextToken();
            _assumption->left = ProcessExpression(ts);
            _assumption->right = ProcessExpression(ts);
            if (ts.GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in assume condition", ts.GetToken());
            }
            ts.NextToken();
            std::shared_ptr <AST::Statement> _statement = ProcessStatement(ts);
            _assumption->statement = _statement;
            _assumption->line_end = _statement->line_end;
            _assumption->position_end = _statement->position_end;
            _assumption->filename = _statement->filename;
            return _assumption;
        }
        if (ts.GetToken().type == TokenType::Free) {
            std::shared_ptr <AST::Free> _free = std::make_shared <AST::Free> ();
            _free->line_begin = ts.GetToken().line_begin;
            _free->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in free statement", ts.GetToken());
            }
            ts.NextToken();
            std::shared_ptr <AST::Expression> _expression = ProcessExpression(ts);
            _free->arg = _expression;
            if (ts.GetToken().type != TokenType::ParenthesisClose) {
                throw AliasException(") expected in free expression", ts.GetToken());
            }
            _free->line_end = ts.GetToken().line_end;
            _free->position_end = ts.GetToken().position_end;
            _free->filename = ts.GetToken().filename;
            ts.NextToken();
            return _free;
        }
        if (ts.GetToken().type == TokenType::Call) {
            std::shared_ptr <AST::FunctionCall> function_call = std::make_shared <AST::FunctionCall> ();
            function_call->line_begin = ts.GetToken().line_begin;
            function_call->position_begin = ts.GetToken().position_begin;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::Identifier) {
                throw AliasException("Identifier expected in function call", ts.GetToken());
            }
            function_call->identifier = ts.GetToken().value_string;
            ts.NextToken();
            if (ts.GetToken().type == TokenType::BracketOpen) {
                ts.NextToken();
                while (true) {
                    if (ts.GetToken().type == TokenType::BracketClose) {
                        ts.NextToken();
                        break;
                    }
                    if (ts.GetToken().type != TokenType::Identifier) {
                        throw AliasException("Idenfier expected in metavariable list", ts.GetToken());
                    }
                    std::string _identifier = ts.GetToken().value_string;
                    ts.NextToken();
                    if (ts.GetToken().type != TokenType::Equal) {
                        throw AliasException("= expected in metavariable list", ts.GetToken());
                    }
                    ts.NextToken();
                    if (ts.GetToken().type != TokenType::Integer) {
                        throw AliasException("Integer expected in metavariable list", ts.GetToken());
                    }
                    int _integer = ts.GetToken().value_int;
                    ts.NextToken();
                    function_call->metavariables.push_back({_identifier, _integer});
                    if (ts.GetToken().type == TokenType::BracketClose) {
                        ts.NextToken();
                        break;
                    }
                    if (ts.GetToken().type != TokenType::Comma) {
                        throw AliasException(", expected in metavariables list", ts.GetToken());
                    }
                    ts.NextToken();
                }
            }
            if (ts.GetToken().type != TokenType::ParenthesisOpen) {
                throw AliasException("( expected in function call", ts.GetToken());
            }
            ts.NextToken();
            while (true) {
                if (ts.GetToken().type == TokenType::ParenthesisClose) {
                    break;
                }
                if (ts.GetToken().type == TokenType::Identifier) {
                    function_call->arguments.push_back(ts.GetToken().value_string);
                }
                else {
                    throw AliasException("Identifier expected in function call", ts.GetToken());
                }
                ts.NextToken();
                if (ts.GetToken().type == TokenType::ParenthesisClose) {
                    break;
                }
                if (ts.GetToken().type != TokenType::Comma) {
                    throw AliasException(", expectred in function call", ts.GetToken());
                }
                ts.NextToken();
            }
            function_call->line_end = ts.GetToken().line_end;
            function_call->position_end = ts.GetToken().position_end;
            function_call->filename = ts.GetToken().filename;
            ts.NextToken();
            return function_call;
        }
        if (ts.GetToken().type == TokenType::Identifier) {
            int line_begin = ts.GetToken().line_begin;
            int position_begin = ts.GetToken().position_begin;
            std::string identifier = ts.GetToken().value_string;
            ts.NextToken();
            if (ts.GetToken().type != TokenType::Assign && ts.GetToken().type != TokenType::Move) {
                throw AliasException(":= or <- expected in assignment or movement statement", ts.GetToken());
            }

            if (ts.GetToken().type == TokenType::Assign) {
                std::shared_ptr <AST::Assignment> assignment = std::make_shared <AST::Assignment> ();
                assignment->line_begin = line_begin;
                assignment->position_begin = position_begin;
                assignment->identifier = identifier;
                ts.NextToken();
                assignment->value = ProcessExpression(ts);
                assignment->line_end = assignment->value->line_end;
                assignment->position_end = assignment->value->position_end;
                assignment->filename = assignment->value->filename;
                return assignment;
            }

            if (ts.GetToken().type == TokenType::Move) {
                ts.NextToken();
                if (ts.GetToken().type == TokenType::String) {
                    std::shared_ptr <AST::MovementString> movement_string = std::make_shared <AST::MovementString> ();
                    movement_string->line_begin = line_begin;
                    movement_string->position_begin = position_begin;
                    movement_string->identifier = identifier;
                    movement_string->line_end = ts.GetToken().line_end;
                    movement_string->position_end = ts.GetToken().position_end;
                    movement_string->filename = ts.GetToken().filename;
                    movement_string->value = ts.GetToken().value_string;
                    ts.NextToken();
                    return movement_string;
                }
                else {
                    std::shared_ptr <AST::Movement> movement = std::make_shared <AST::Movement> ();
                    movement->line_begin = line_begin;
                    movement->position_begin = position_begin;
                    movement->identifier = identifier;
                    movement->value = ProcessExpression(ts);
                    movement->line_end = movement->value->line_end;
                    movement->position_end = movement->value->position_end;
                    movement->filename = movement->value->filename;
                    return movement;
                }
            }
        }
        throw AliasException("Statement expected", ts.GetToken());
    }

    std::shared_ptr <AST::Node> Process(const std::vector <Token> tokens) {
        TokenStream token_stream(tokens);
        return ProcessProgram(token_stream);
    }
}
