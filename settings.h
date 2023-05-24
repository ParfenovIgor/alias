#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <string>

namespace Settings {
    bool GetServer();
    void SetServer(bool state);
    bool GetCompile();
    void SetCompile(bool state);
    bool GetLink();
    void SetLink(bool state);
    std::string GetOutputFilename();
    void SetOutputFilename(std::string state);
}

#endif // STATE_H_INCLUDED
