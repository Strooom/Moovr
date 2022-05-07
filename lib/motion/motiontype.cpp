#include "motiontype.h"

const char* toString(motionType aMotionType) {
     switch (aMotionType) {
        case motionType::traverse:
            return "traverse";
            break;
        case motionType::feedLinear:
            return "feedLinear";
            break;
        case motionType::feedHelicalCW:
            return "feedHelicalCW";
            break;
        case motionType::feedHelicalCCW:
            return "feedHelicalCCW";
            break;
        case motionType::pauseAndResume:
            return "pauseAndResume";
            break;
        case motionType::pause:
            return "pause";
            break;
        case motionType::none:
            return "none";
            break;
        default:
            return "unknown";
            break;
    }
}

