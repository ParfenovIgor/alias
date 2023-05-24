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
        token_stream = Lexer::Process(buffer.str(), filename);
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
        std::string output_filename = Settings::GetOutputFilename();
        std::ofstream file(output_filename + ".asm");
        AST::Compile(node, file);
        file.close();
        
        if (Settings::GetLink()){
            std::string cmd = "nasm -f elf32 " + output_filename + ".asm -o " + output_filename + ".o";
            system(cmd.c_str());
            cmd = "gcc -m32 " + output_filename + ".o -no-pie -o " + output_filename;
            system(cmd.c_str());
        }
    }

    return 0;
}
