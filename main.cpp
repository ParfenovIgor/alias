#include <sstream>
#include <fstream>
#include <iostream>

#include "lexer.h"
#include "syntax.h"
#include "exception.h"

void help() {
    std::cout << "HELP" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        help();
    }
    else {
        std::ifstream fin(argv[1]);
        if (!fin) {
            help();
        }
        else {
            std::stringstream buffer;
            buffer << fin.rdbuf();
            std::vector <Token> token_stream;
            std::shared_ptr <AST::Node> node;
            try {
                token_stream = Lexer::process(buffer.str());
            }
            catch (AliasException &ex) {
                std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.position_begin + ex.length << std::endl;
                std::cout << "Lexer Error: " << ex.value << std::endl;
            }
            try {
                node = Syntax::Process(token_stream);
            }
            catch (AliasException &ex) {
                std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.position_begin + ex.length << std::endl;
                std::cout << "Syntax Error: " << ex.value << std::endl;
            }
            try {
                node->Validate();
            }
            catch (AliasException &ex) {
                std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.line_end + 1 << ':' << ex.position_end + 1 << std::endl;
                std::cout << "Semantic Error: " << ex.value << std::endl;
            }
        }
    }

    return 0;
}
