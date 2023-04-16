#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer.h"
#include "syntax.h"
#include "validator.h"
#include "compile.h"
#include "exception.h"
#include "settings.h"
#include "process.h"

std::shared_ptr <AST::Node> Parse(std::string filename) {
    std::ifstream fin(filename);
    if (!fin) {
        std::cerr << "Could not open file " << filename << "\n";
        exit(1);
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();
    std::vector <Token> token_stream;
    std::shared_ptr <AST::Node> node;
    try {
        token_stream = Lexer::process(buffer.str(), filename);
    }
    catch (AliasException &ex) {
        std::cout << "Error" << std::endl;
        std::cout << ex.filename << std::endl;
        std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.line_end + 1 << ':' << ex.position_end + 1 << std::endl;
        std::cout << "Lexer Error: " << ex.value << std::endl;
        exit(1);
    }

    try {
        node = Syntax::Process(token_stream);
    }
    catch (AliasException &ex) {
        std::cout << "Error" << std::endl;
        std::cout << ex.filename << std::endl;
        std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.line_end + 1 << ':' << ex.position_end + 1 << std::endl;
        std::cout << "Syntax Error: " << ex.value << std::endl;
        exit(1);
    }

    return node;
}

int Process(std::string filename) {
    std::shared_ptr <AST::Node> node = Parse(filename);

    try {
        AST::Validate(node);
        if (Settings::GetServer()) {
            AST::PrintStatesLog();
        }
    }
    catch (AliasException &ex) {
        std::cout << "Error" << std::endl;
        std::cout << ex.filename << std::endl;
        std::cout << ex.line_begin + 1 << ':' << ex.position_begin + 1 << '-' << ex.line_end + 1 << ':' << ex.position_end + 1 << std::endl;
        std::cout << "Semantic Error: " << ex.value << std::endl;
        exit(1);
    }

    if (Settings::GetCompile()) {
        std::ofstream file("program.asm");
        AST::Compile(node, file);
        file.close();
        system("nasm -f elf32 program.asm -o program.o");
        system("gcc -m32 program.o -no-pie -o program");
    }

    return 0;
}