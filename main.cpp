#include <iostream>

#include "process.h"
#include "settings.h"

void help() {
    std::cout << "HELP" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        help();
        return 0;
    }
    else {
        for (int i = 1; i < argc; i++) {
            std::string arg(argv[i]);
            if (arg == "-v") {
                Settings::SetServer(true);
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
                Settings::SetNoMain(true);
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
