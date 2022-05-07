#include "motionstrategy.h"

const char* toString(motionStrategy aStrategy)
{
    switch (aStrategy) {
        case motionStrategy::minimizeSpeed:
            return "minimizeSpeed";
            break;
        case motionStrategy::maximizeSpeed:
            return "minimizeSpeed";
            break;
        default:
            return "unknown";
            break;
    }
}