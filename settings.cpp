#include "settings.h"

namespace Settings {
    bool States = false;
    bool Compile = false;
    bool Assemble = false;
    bool Link = false;
    bool TopMain = false;
    std::string Filename;
    std::string OutputFilename;

    bool GetStates() {
        return States;
    }

    void SetStates(bool state) {
        States = state;
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

    bool GetTopMain() {
        return TopMain;
    }

    void SetTopMain(bool state) {
        TopMain = state;
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
