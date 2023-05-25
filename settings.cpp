#include "settings.h"

namespace Settings {
    bool Server = false;
    bool Compile = false;
    bool Assemble = false;
    bool Link = false;
    bool NoMain = false;
    std::string Filename;
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

    bool GetAssemble() {
        return Assemble;
    }

    void SetAssemble(bool state) {
        Assemble = state;
    }

    bool GetLink() {
        return Link;
    }

    void SetLink(bool state) {
        Link = state;
    }

    bool GetNoMain() {
        return NoMain;
    }

    void SetNoMain(bool state) {
        NoMain = state;
    }

    std::string GetFilename() {
        return Filename;
    }

    void SetFilename(std::string state) {
        Filename = state;
    }

    std::string GetOutputFilename() {
        return OutputFilename;
    }

    void SetOutputFilename(std::string state) {
        OutputFilename = state;
    }
}
