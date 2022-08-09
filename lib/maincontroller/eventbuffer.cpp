#include "eventbuffer.h"

void eventBuffer::pushEvent(event theEvent) {
    if (event::none != theEvent) {
        if (level < length) {
            theEventBuffer[(head + level) % length] = theEvent;        // write new event at next writeIndex = readIndex + Level
            level++;                                                   // adjust level to one item more
            if (level > levelMax) {
                levelMax = level;
            }
        } else {
            lastError = event::eventBufferOverflow;
        }
    }
}

event eventBuffer::popEvent() {
    event theEvent = event::none;
    if (level > 0U) {
        theEvent = theEventBuffer[head];        // read the oldest event
        head     = (head + 1U) % length;        // advance readIndex to next position
        level--;                                // adjust level to one item less
    } else {
        lastError = event::eventBufferUnderflow;
    }
    return theEvent;
}

bool eventBuffer::hasEvents() {
    bool hasEvents = (level > 0U);        // make thread-safe copy
    return hasEvents;
}

uint32_t eventBuffer::getLevelMax() {
    uint32_t getLevelMax = levelMax;        // make thread-safe copy
    levelMax             = 0U;              // reset level after reading it
    return getLevelMax;
}

event eventBuffer::getLastError() {
    event result = lastError;
    lastError    = event::none;
    return result;
}
