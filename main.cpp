#include <iostream>

#include "lexer.h"
#include "syntax.h"
#include "validator.h"
#include "compile.h"
#include "exception.h"
#include "process.h"
#include "state.h"

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
            if (arg == "-S") {
                Settings::GetTarget() = Settings::Target::Server;
            }
            else {
                filename = arg;
            }
        }

        if (filename.empty()) {
            help();
            return 0;
        }

        return process(filename);
    }
}
