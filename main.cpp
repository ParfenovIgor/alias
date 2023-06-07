#include <iostream>

#include "process.h"
#include "settings.h"

void help() {
    std::cout << "Syntax: calias [flags] file [flags]\n";
    std::cout << "Flags:\n";
    std::cout << "  -s        Print states collected during validation.\n";
    std::cout << "  -c        Compile program to Asm code.\n";
    std::cout << "  -a        Compile program and assemble it using nasm to object file.\n";
    std::cout << "  -l        Compile, assemble and link program using gcc to executable file.\n";
    std::cout << "  -m        Disable top level main function.\n";
    std::cout << "  -o        Set output file name. File name has to follow this flag.\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        help();
        return 0;
    }
    else {
        for (int i = 1; i < argc; i++) {
            std::string arg(argv[i]);
            if (arg == "-s") {
                Settings::SetStates(true);
            }
            else if(arg == "-c") {
                Settings::SetCompile(true);
            }
            else if(arg == "-a") {
                Settings::SetAssemble(true);
            }
            else if (arg == "-l") {
                Settings::SetLink(true);
            }
            else if (arg == "-m") {
                Settings::SetTopMain(true);
            }
            else if (arg == "-o") {
                if (i + 1 == argc) {
                    std::cout << "Filename has to be specified after -o flag" << std::endl;
                    return 1;
                }
                std::string str(argv[i + 1]);
                Settings::SetOutputFilename(str);
                i++;
            }
            else {
                Settings::SetFilename(arg);
            }
        }

        if (Settings::GetFilename().empty()) {
            help();
            return 0;
        }

        return Process();
    }
}
