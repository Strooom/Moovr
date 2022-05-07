#include "plane.h"

const char* toString(plane aPlane) {
    switch (aPlane) {
        case plane::XY:
            return "XY";
            break;
        case plane::YZ:
            return "YZ";
            break;
        case plane::ZX:
            return "ZX";
            break;
        default:
            return "unknown";
            break;
    }
}
