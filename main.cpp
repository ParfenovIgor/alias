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
        std::string filename;
        std::string output_filename;
        for (int i = 1; i < argc; i++) {
            std::string arg(argv[i]);
            if (arg == "-v") {
                Settings::SetServer(true);
            }
            else if(arg == "-c") {
                Settings::SetCompile(true);
            }
            else if (arg == "-l") {
                Settings::SetLink(true);
            }
            else if (arg == "-o") {
                if (i + 1 == argc) {
                    std::cout << "Filename has to be specified after -o flag" << std::endl;
                    return 1;
                }
                std::string str(argv[i + 1]);
                output_filename = str;
                i++;
            }
            else {
                filename = arg;
                if (output_filename.empty()) {
                    output_filename = arg + ".";
                    for (int j = 0; j < (int)output_filename.size(); j++) {
                        if (output_filename[j] == '.') {
                            output_filename = output_filename.substr(0, j);
                            break;
                        }
                    }
                }
            }
        }
        Settings::SetOutputFilename(output_filename);
        std::cout << output_filename << std::endl;

        if (filename.empty()) {
            help();
            return 0;
        }

        return Process(filename);
    }
}
