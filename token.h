#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

enum class TokenType {
    Identifier,
    BraceOpen,
    BraceClose,
    ParenthesisOpen,
    ParenthesisClose,
    BracketOpen,
    BracketClose,
    Asm,
    Include,
    Comma,
    Colon,
    Semicolon,
    Int,
    Ptr,
    If,
    Else,
    While,
    Func,
    Proto,
    Def,
    Const,
    Assign,
    Move,
    Assume,
    Integer,
    String,
    Dereference,
    Caret,
    Plus,
    Minus,
    Mult,
    Div,
    Less,
    Equal,
    Alloc,
    Free,
    Call,
    Eof,
};

class Token {
public:
    Token(TokenType _type, int _line_begin, int _position_begin, int _line_end, int _position_end, std::string _filename) {
        type = _type;
        line_begin = _line_begin;
        position_begin = _position_begin;
        line_end = _line_end;
        position_end = _position_end;
        filename = _filename;
    }
    Token(TokenType _type, int _value_int, int _line_begin, int _position_begin, int _line_end, int _position_end, std::string _filename) {
        type = _type;
        value_int = _value_int;
        line_begin = _line_begin;
        position_begin = _position_begin;
        line_end = _line_end;
        position_end = _position_end;
        filename = _filename;
    }
    Token(TokenType _type, std::string _value_string, int _line_begin, int _position_begin, int _line_end, int _position_end, std::string _filename) {
        type = _type;
        value_string = _value_string;
        line_begin = _line_begin;
        position_begin = _position_begin;
        line_end = _line_end;
        position_end = _position_end;
        filename = _filename;
    }

    TokenType type;
    int value_int;
    std::string value_string;
    int line_begin, position_begin, line_end, position_end;
    std::string filename;
};

#endif // TOKEN_H_INCLUDED
