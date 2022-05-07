#include "probingstate.h"

const char* toString(probingState aProbingState) {
    switch (aProbingState) {
        case probingState::lost:
            return "lost";
            break;
        case probingState::search:
            return "search";
            break;
        case probingState::found:
            return "found";
            break;
        default:
            return "unknown";
            break;
    }
}