#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include <stdexcept>
#include "token.h"
#include "ast.h"

class AliasException : public std::exception {
public:
    AliasException(std::string _value, int _line_begin, int _position_begin, int _length) {
        value = _value;
        line_begin = _line_begin;
        position_begin = _position_begin;
        length = _length;
        line_end = 0;
        position_end = 0;
    }

    AliasException(std::string _value, int _line_begin, int _position_begin, int _line_end, int _position_end) {
        value = _value;
        line_begin = _line_begin;
        position_begin = _position_begin;
        length = 0;
        line_end = _line_end;
        position_end = _position_end;
    }

    AliasException(std::string _value, Token _token) {
        value = _value;
        line_begin = _token.line;
        position_begin = _token.position;
        length = _token.length;
        line_end = 0;
        position_end = 0;
    }

    AliasException(std::string _value, AST::Node *_node) {
        value = _value;
        line_begin = _node->line_begin;
        position_begin = _node->position_begin;
        length = 0;
        line_end = _node->line_end;
        position_end = _node->position_end;
    }

    std::string value;
    int line_begin, position_begin, length, line_end, position_end;
};

#endif // EXCEPTION_H_INCLUDED
