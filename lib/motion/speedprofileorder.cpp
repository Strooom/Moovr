#include "speedprofileorder.h"

const char* toString(speedProfileOrder aMotionSpeedProfileOrder) {
    switch (aMotionSpeedProfileOrder) {
        case speedProfileOrder::firstOrder:
            return "1st order";
            break;
        case speedProfileOrder::secondOrder:
            return "2nd order";
            break;
        default:
            return "unknown";
            break;
    }
}
