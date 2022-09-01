#include "homingstate.h"

const char* toString(homingState aHomingState) {
    switch (aHomingState) {
        case homingState::lost:
            return "lost";
            break;
        case homingState::stopping:
            return "stopping";
            break;
        case homingState::closing:
            return "closing";
            break;
        case homingState::closedWaitForStop:
            return "closedWaitForStop";
            break;
        case homingState::opening:
            return "opening";
            break;
        case homingState::openedWaitForStop:
            return "openedWaitForStop";
            break;
        case homingState::offsettingWaitForStop:
            return "offsettingWaitForStop";
            break;
        case homingState::found:
            return "found";
            break;
        default:
            return "unknown";
            break;
    }
}
