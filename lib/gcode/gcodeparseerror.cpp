#include "gcodeparseerror.h"

const char* toString(gCodeParseError anError) {
    switch (anError) {
        case gCodeParseError::None:
            return "None";
            break;
            // TODO : add all other errors
        default:
            return "unknown";
            break;
    }
}
