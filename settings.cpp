#include "settings.h"

namespace Settings {
    bool Server = false;
    bool Compile = false;

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
}
