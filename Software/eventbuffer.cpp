// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "eventbuffer.h"
#include "logging.h"

extern uLog theLog;

// TODO : check if this threadprotection is needed... Where do we post events from other threads ?

void eventBuffer::pushEvent(event theEvent) {
    if (event::none != theEvent) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        noInterrupts();
#endif
        if (bufferLevel < bufferLength) {
            theEventBuffer[(bufferReadIndex + bufferLevel) % bufferLength] = theEvent;        // write new event at next writeIndex = readIndex + Level
            bufferLevel++;                                                                      // adjust level to one item more
            if (bufferLevel > bufferLevelMax) {
                bufferLevelMax = bufferLevel;
            }
        } else {
            theLog.output(loggingLevel::Error, "Eventbuffer Overflow");
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
    if (bufferLevel > 0) {
        theEvent        = theEventBuffer[bufferReadIndex];                     // read the oldest event
        bufferReadIndex = (bufferReadIndex + 1) % bufferLength;        // advance readIndex to next position
        bufferLevel--;                                                      // adjust level to one item less
    } else {
        theLog.output(loggingLevel::Error, "Eventbuffer Underflow");
    }
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    return theEvent;
}

bool eventBuffer::hasEvents() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    bool hasEvents = (bufferLevel > 0);
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    return hasEvents;
}

uint32_t eventBuffer::getBufferLevelMax() {
    // TODO : check if we need to disable/re-enable interrupts here
    uint32_t tmpLevel = bufferLevelMax;
    bufferLevelMax    = 0;        // reset level when after reading it
    return tmpLevel;
}
