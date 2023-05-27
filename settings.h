#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <string>

namespace Settings {
    bool GetStates();
    void SetStates(bool state);
    bool GetCompile();
    void SetCompile(bool state);
    bool GetAssemble();
    void SetAssemble(bool state);
    bool GetLink();
    void SetLink(bool state);
    bool GetTopMain();
    void SetTopMain(bool state);
    std::string GetFilename();
    void SetFilename(std::string state);
    std::string GetOutputFilename();
    void SetOutputFilename(std::string state);
}

#endif // STATE_H_INCLUDED
