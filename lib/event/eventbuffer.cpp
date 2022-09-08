#include "eventbuffer.h"

void eventBuffer::initialize() {
    head      = 0;
    level     = 0;
    maxLevel  = 0;
    lastError = event::none;
}

void eventBuffer::pushEvent(event theEvent) {
    if (event::none != theEvent) {
        if (level < length) {
            theEventBuffer[(head + level) % length] = theEvent;        // write new event at next writeIndex = readIndex + Level
            level++;                                                   // adjust level to one item more
            if (level > maxLevel) {
                maxLevel = level;
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
    bool result = (level > 0U);        // make thread-safe copy
    return result;
}

uint32_t eventBuffer::getMaxLevel() {
    uint32_t result = maxLevel;        // make thread-safe copy
    maxLevel             = 0U;              // reset level after reading it
    return result;
}

event eventBuffer::getLastError() {
    event result = lastError;
    lastError    = event::none;
    return result;
}
