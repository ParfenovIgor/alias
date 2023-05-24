#include "settings.h"

namespace Settings {
    bool Server = false;
    bool Compile = false;
    bool Link = false;
    std::string OutputFilename;

    bool GetServer() {
        return Server;
    }

    void SetServer(bool state) {
        Server = state;
    }

    bool GetCompile() {
        return Compile;
    }

    void SetCompile(bool state) {
        Compile = state;
    }

    bool GetLink() {
        return Link;
    }

    void SetLink(bool state) {
        Link = state;
    }

    std::string GetOutputFilename() {
        return OutputFilename;
    }

    void SetOutputFilename(std::string state) {
        OutputFilename = state;
    }
}
