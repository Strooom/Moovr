#include "plane.h"

const char* toString(plane aPlane) {
    switch (aPlane) {
        case plane::XY:
            return "XY";
            break;
        default:
            return "unknown";
            break;
    }
}
