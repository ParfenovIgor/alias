#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <string>
#include <vector>

#include "token.h"
#include "exception.h"

namespace Lexer {
    bool is_alpha(char c) {
        return ((c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') || c == '_');
    }

    bool is_digit(char c) {
        return (c >= '0' && c <= '9');
    }

    bool is_reserved_word(std::string &str, std::string word, int i) {
        size_t n = word.size();
        return (i + n <= str.size() && str.substr(i, n) == word &&
                (i + n == str.size() ||
                (!is_digit(str[i + n]) && !is_alpha(str[i + n]))));
    }

    std::vector <Token> process(std::string str) {
        std::vector <Token> token_stream;
        int line = 0, position = 0;
        for (size_t i = 0; i < str.size();) {
            if (is_reserved_word(str, "int", i)) {
                token_stream.push_back(Token(TokenType::Int, line, position, 3));
                i += 3;
                position += 3;
            }
            else if (is_reserved_word(str, "ptr", i)) {
                token_stream.push_back(Token(TokenType::Ptr, line, position, 3));
                i += 3;
                position += 3;
            }
            else if (is_reserved_word(str, "if", i)) {
                token_stream.push_back(Token(TokenType::If, line, position, 2));
                i += 2;
                position += 2;
            }
            else if (is_reserved_word(str, "else", i)) {
                token_stream.push_back(Token(TokenType::Else, line, position, 4));
                i += 4;
                position += 4;
            }
            else if (is_reserved_word(str, "while", i)) {
                token_stream.push_back(Token(TokenType::While, line, position, 5));
                i += 5;
                position += 5;
            }
            else if (is_reserved_word(str, "func", i)) {
                token_stream.push_back(Token(TokenType::Func, line, position, 4));
                i += 4;
                position += 4;
            }
            else if (is_reserved_word(str, "def", i)) {
                token_stream.push_back(Token(TokenType::Def, line, position, 3));
                i += 3;
                position += 3;
            }
            else if (is_reserved_word(str, "assume", i)) {
                token_stream.push_back(Token(TokenType::Assume, line, position, 6));
                i += 6;
                position += 6;
            }
            else if (is_reserved_word(str, "alloc", i)) {
                token_stream.push_back(Token(TokenType::Alloc, line, position, 5));
                i += 5;
                position += 5;
            }
            else if (is_reserved_word(str, "free", i)) {
                token_stream.push_back(Token(TokenType::Free, line, position, 4));
                i += 4;
                position += 4;
            }
            else if (is_reserved_word(str, "call", i)) {
                token_stream.push_back(Token(TokenType::Call, line, position, 4));
                i += 4;
                position += 4;
            }
            else if (i + 2 <= str.size() && str.substr(i, 2) == ":=") {
                token_stream.push_back(Token(TokenType::Assign, line, position, 2));
                i += 2;
                position += 2;
            }
            else if (i + 2 <= str.size() && str.substr(i, 2) == "<-") {
                token_stream.push_back(Token(TokenType::Move, line, position, 2));
                i += 2;
                position += 2;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == ",") {
                token_stream.push_back(Token(TokenType::Comma, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == ";") {
                token_stream.push_back(Token(TokenType::Semicolon, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "{") {
                token_stream.push_back(Token(TokenType::BraceOpen, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "}") {
                token_stream.push_back(Token(TokenType::BraceClose, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "(") {
                token_stream.push_back(Token(TokenType::ParenthesisOpen, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == ")") {
                token_stream.push_back(Token(TokenType::ParenthesisClose, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "$") {
                token_stream.push_back(Token(TokenType::Dereference, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "+") {
                token_stream.push_back(Token(TokenType::Plus, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "-") {
                if (i + 2 <= str.size() && is_digit(str[i + 1])) {
                    int l = i;
                    i++;
                    i++;
                    while (i + 1 <= str.size() && is_digit(str[i])) i++;
                    int r = i - 1;
                    token_stream.push_back(Token(TokenType::Integer, atoi(str.substr(l, r - l + 1).c_str()), line, position, r - l + 1));
                    position += r - l + 1;
                }
                else {
                    token_stream.push_back(Token(TokenType::Minus, line, position, 1));
                    i += 1;
                    position += 1;
                }
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "<") {
                token_stream.push_back(Token(TokenType::Less, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "=") {
                token_stream.push_back(Token(TokenType::Equal, line, position, 1));
                i += 1;
                position += 1;
            }
            else if (i + 2 <= str.size() && str.substr(i, 2) == "//") {
                i++;
                while(i < str.size() && str[i] != '\n') i++;
                i = std::min(i + 1, str.size());
                position = 0;
                line++;
            }
            else if (i + 2 <= str.size() && str.substr(i, 2) == "/*") {
                i += 2;
                position += 2;
                while(i + 2 <= str.size() && str.substr(i, 2) != "*/") {
                    i++;
                    position++;
                    if (str[i] == '\n') {
                        position = -1;
                        line++;
                    }
                }
                position += 2;
                i = std::min(i + 2, str.size());
            }
            else if (i + 1 <= str.size() && str.substr(i, 1) == "\"") {
                i++;
                int l_line = line;
                int l_position = position;
                position++;
                int l = i;
                while(i + 1 <= str.size() && str.substr(i, 1) != "\"") {
                    i++;
                    position++;
                    if (str[i] == '\n') {
                        position = -1;
                        line++;
                    }
                }
                if (i == str.size())
                    throw AliasException("Non closed string", l_line, l_position, 1);
                int r = i - 1;
                token_stream.push_back(Token(TokenType::String, str.substr(l, r - l + 1), l_line, l_position, r - l + 1));
                position++;
                i++;
            }
            else if (is_digit(str[i])) {
                int l = i;
                i++;
                while (i + 1 <= str.size() && is_digit(str[i])) i++;
                int r = i - 1;
                token_stream.push_back(Token(TokenType::Integer, atoi(str.substr(l, r - l + 1).c_str()), line, position, r - l + 1));
                position += r - l + 1;
            }
            else if (is_alpha(str[i])) {
                int l = i;
                i++;
                while (i + 1 <= str.size() && (is_alpha(str[i]) || is_digit(str[i]))) i++;
                int r = i - 1;
                token_stream.push_back(Token(TokenType::Identifier, str.substr(l, r - l + 1), line, position, r - l + 1));
                position += r - l + 1;
            }
            else if (str[i] == ' ' || str[i] == '\t') {
                i++;
                position++;
            }
            else if (str[i] == '\n') {
                i++;
                position = 0;
                line++;
            }
            else {
                throw AliasException("Unexpected symbol", line, position, 1);
            }
        }

        token_stream.push_back(Token(TokenType::Eof, line, position, 1));

        return token_stream;
    }
}

#endif // LEXER_H_INCLUDED
