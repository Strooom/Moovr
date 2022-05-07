#include "rotationdirection.h"


const char* toString(rotationDirection aRotationDirection) {
     switch (aRotationDirection) {
        case rotationDirection::clockwise:
            return "clockwise";
            break;
        case rotationDirection::counterClockwise:
            return "counterClockwise";
            break;
        default:
            return "unknown";
            break;
    }
}