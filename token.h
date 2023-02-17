#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

enum class TokenType {
    Identifier,
    BraceOpen,
    BraceClose,
    ParenthesisOpen,
    ParenthesisClose,
    Semicolon,
    Int,
    Ptr,
    If,
    Else,
    While,
    Def,
    Assign,
    Move,
    Assume,
    Integer,
    String,
    Plus,
    Minus,
    Less,
    Equal,
    Alloc,
    Free,
    Eof,
};

class Token {
public:
    Token(TokenType _type, int _line, int _position, int _length) {
        type = _type;
        line = _line;
        position = _position;
        length = _length;
    }
    Token(TokenType _type, int _value_int, int _line, int _position, int _length) {
        type = _type;
        value_int = _value_int;
        line = _line;
        position = _position;
        length = _length;
    }
    Token(TokenType _type, std::string _value_string, int _line, int _position, int _length) {
        type = _type;
        value_string = _value_string;
        line = _line;
        position = _position;
        length = _length;
    }

    TokenType type;
    int value_int;
    std::string value_string;
    int line, position, length;
};

#endif // TOKEN_H_INCLUDED
