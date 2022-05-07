#include "mainstate.h"

const char *toString(mainState aState) {
    switch (aState) {
        case mainState::ready:
            return "ready";
            break;
        case mainState::homing:
            return "homing";
            break;
        case mainState::probing:
            return "probing";
            break;
        case mainState::running:
            return "running";
            break;
        case mainState::error:
            return "error";
            break;
        default:
            return "unknown";
            break;
    }
}
