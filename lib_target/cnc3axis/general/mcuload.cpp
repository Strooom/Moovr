#include "mcuload.h"
#include "machineproperties.h"

void mcuLoad::start() {
    sampleTimer.start(minStepBufferTotalTimeMs);
}

void mcuLoad::run() {
    if (sampleTimer.expired()) {
        load = (10000.0f / static_cast<float>(loopCounter));
        if (load > maxLoad) {
            maxLoad = load;
        }
        loopCounter = 0;
    } else {
        loopCounter++;
    }
}

float mcuLoad::getLoad() {
    return load;
}

float mcuLoad::getMaxLoad() {
    float result = maxLoad;
    maxLoad      = 0.0F;
    return result;
}
