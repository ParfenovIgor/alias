#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

namespace Settings {
    enum class Target {
        Validate,
        Server,
        Compile,
    };

    Target &GetTarget();
}

#endif // STATE_H_INCLUDED
