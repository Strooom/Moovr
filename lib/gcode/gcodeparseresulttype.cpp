#include "gcodeparseresulttype.h"

const char* toString(gCodeParseResultType aParseResultType) {
    switch (aParseResultType) {
        case gCodeParseResultType::EmptyBlock:
            return "EmptyBlock";
            break;
        case gCodeParseResultType::OkContextUpdateOnly:
            return "OkContextUpdateOnly";
            break;
        case gCodeParseResultType::OkContextUpdateAndMotion:
            return "OkContextUpdateAndMotion";
            break;
        // TODO : complete the extra types
        default:
            return "unknown";
            break;
    }
}