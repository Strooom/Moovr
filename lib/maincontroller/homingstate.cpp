#include "homingstate.h"

const char* toString(homingState aHomingState) {
    switch (aHomingState) {
        case homingState::lost:
            return "lost";
            break;
        case homingState::closingZ:
            return "closingZ";
            break;
        case homingState::closedZ:
            return "closedZ";
            break;
        case homingState::openingZ:
            return "openingZ";
            break;
        case homingState::openedZ:
            return "openedZ";
            break;
        case homingState::closingY:
            return "closingY";
            break;
        case homingState::closedY:
            return "closedY";
            break;
        case homingState::openingY:
            return "openingY";
            break;
        case homingState::openedY:
            return "openedY";
            break;
        case homingState::closingX:
            return "closingX";
            break;
        case homingState::closedX:
            return "closedX";
            break;
        case homingState::openingX:
            return "openingX";
            break;
        case homingState::openedX:
            return "openedX";
            break;
        case homingState::found:
            return "found";
            break;
        default:
            return "unknown";
            break;
    }
}
