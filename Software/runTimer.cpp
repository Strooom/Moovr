#include "runTimer.h"

intervalTimer::intervalTimer(unsigned long interval) : timerInterval{interval} {
}
bool intervalTimer::expired() {
#ifndef WIN32
    if (millis() - timerValue > timerInterval) {        // check if actual time >= previous time + interval
        timerValue += timerInterval;                    // set new compare time
        return true;
    } else {
        return false;
    }
#else
    return false;
#endif
}

bool singleTimer::expired() {
#ifndef WIN32
    if (running) {
        if (millis() - timerStart > timerDuration) {        // check if actual time >= startTime + duration
            running = false;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
#else
    return false;
#endif
}
void singleTimer::start(unsigned long theTimerDuration) {        // re-start a new timeout
#ifndef WIN32
    timerStart    = millis();
    timerDuration = theTimerDuration;
    running       = true;
#endif
}

counter::counter(uint32_t theMaxCount) : maxCount{theMaxCount} {
}
uint32_t counter::value() const {
    return count;
}
bool counter::expired() const {
    return (count >= maxCount);
}
void counter::increment() {
    count++;
}
void counter::reset() {
    count = 0;
}