#include "state.h"

namespace Settings {
    Target target = Target::Validate;
    Target &GetTarget() {
        return target;
    }
}
