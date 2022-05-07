#include "motionstate.h"

const char* toString(motionState theMotionState)
{
    switch (theMotionState) {
        case motionState::running:
            return "running";
            break;
        case motionState::stopping:
            return "stopping";
            break;
        case motionState::stopped:
            return "stopped";
            break;
        default:
            return "unknown";
            break;
    }
}