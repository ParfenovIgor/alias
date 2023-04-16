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
        for (int i = 1; i < argc; i++) {
            std::string arg(argv[i]);
            if (arg == "-v") {
                Settings::SetServer(true);
            }
            else if(arg == "-c") {
                Settings::SetCompile(true);
            }
            else {
                filename = arg;
            }
        }

        if (filename.empty()) {
            help();
            return 0;
        }

        return Process(filename);
    }
}
