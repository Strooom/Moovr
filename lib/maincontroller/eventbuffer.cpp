#include "eventbuffer.h"

eventBuffer::eventBuffer() {}

void eventBuffer::pushEvent(event theEvent) {
    if (event::none != theEvent) {

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        noInterrupts();
#endif
        if (level < length) {
            theEventBuffer[(head + level) % length] = theEvent;        // write new event at next writeIndex = readIndex + Level
            level++;                                                   // adjust level to one item more
            if (level > levelMax) {
                levelMax = level;
            }
        } else {
            //theLog.output(loggingLevel::Error, "Eventbuffer Overflow");
        }
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        interrupts();
#endif
    }
}

event eventBuffer::popEvent() {
    event theEvent = event::none;
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    if (level > 0U) {
        theEvent = theEventBuffer[head];        // read the oldest event
        head     = (head + 1U) % length;        // advance readIndex to next position
        level--;                                // adjust level to one item less
    } else {
        //theLog.output(loggingLevel::Error, "Eventbuffer Underflow");
    }
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    //theLog.snprintf(loggingLevel::Debug, "pop event : %s", toString(theEvent));
    return theEvent;
}

bool eventBuffer::hasEvents() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    bool hasEvents = (level > 0U);                          // make thread-safe copy
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    return hasEvents;
}

uint32_t eventBuffer::getLevelMax() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    uint32_t getLevelMax = levelMax;                        // make thread-safe copy
    levelMax             = 0U;                              // reset level after reading it
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    return getLevelMax;
}

