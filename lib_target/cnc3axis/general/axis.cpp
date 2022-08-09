#include "axis.h"

const char* toString(axis anAxis) {
    switch (anAxis) {
        case axis::X:
            return "X";
            break;
        case axis::Y:
            return "Y";
            break;
        case axis::Z:
            return "Z";
            break;
        default:
            return "unknown";
            break;
    }
}
