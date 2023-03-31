#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED

#include <fstream>
#include <sstream>

#include "state.h"

int process(std::string filename) {
    std::ifstream fin(filename);
    if (!fin) {
        std::cerr << "Could not open file " << filename << "\n";
        return -1;
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();
    std::vector <Token> token_stream;
    std::shared_ptr <AST::Node> node;
    try {
        token_stream = Lexer::process(buffer.str(), filename);
    }
    catch (AliasException &ex) {
        std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.line_end + 1 << ':' << ex.position_end + 1 << std::endl;
        std::cout << "Lexer Error: " << ex.value << std::endl;
        return 1;
    }

    try {
        node = Syntax::Process(token_stream);
    }
    catch (AliasException &ex) {
        std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.line_end + 1 << ':' << ex.position_end + 1 << std::endl;
        std::cout << "Syntax Error: " << ex.value << std::endl;
        return 1;
    }

    try {
        AST::Validate(node);
        if (Settings::GetTarget() == Settings::Target::Server) {
            AST::PrintStatesLog();
        }
    }
    catch (AliasException &ex) {
        std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.line_end + 1 << ':' << ex.position_end + 1 << std::endl;
        std::cout << "Semantic Error: " << ex.value << std::endl;
        return 1;
    }

    std::ofstream file("program.asm");
    AST::Compile(node, file);
    file.close();
    system("nasm -f elf32 program.asm -o program.o");
    system("gcc -m32 program.o -no-pie -o program");

    return 0;
}

#endif // PROCESS_H_INCLUDED