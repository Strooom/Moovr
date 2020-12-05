// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "event.h"
#include "logging.h"

extern uLog theLog;

void EventBuffer::pushEvent(Event theEvent) {
    if (Event::none != theEvent) {
#ifndef WIN32
        cli();        // no Interrupts : this function can be called from main thread and fron interrupt handlers
#endif
        if (bufferLevel < eventBufferLength) {
            eventBuffer[(bufferReadIndex + bufferLevel) % eventBufferLength] = theEvent;        // write new event at next writeIndex = readIndex + Level
            bufferLevel++;                                                                      // adjust level to one item more
            if (bufferLevel > bufferLevelMax) {
                bufferLevelMax = bufferLevel;
            }
        } else {
            theLog.output(loggingLevel::Error, "Eventbuffer Overflow");
        }
#ifndef WIN32
        sei();        // re-enable interrupts
#endif
    }
}

Event EventBuffer::popEvent() {
    Event theEvent = Event::none;
#ifndef WIN32
    cli();        // no Interrupts : this function can be called from main thread and fron interrupt handlers
#endif
    if (bufferLevel > 0) {
        theEvent        = eventBuffer[bufferReadIndex];                     // read the oldest event
        bufferReadIndex = (bufferReadIndex + 1) % eventBufferLength;        // advance readIndex to next position
        bufferLevel--;                                                      // adjust level to one item less
    } else {
        theLog.output(loggingLevel::Error, "Eventbuffer Underflow");
    }
#ifndef WIN32
    sei();        // re-enable interrupts
#endif
    return theEvent;
}

bool EventBuffer::hasEvents() {
#ifndef WIN32
    cli();        // critical section
#endif
    bool hasEvents = (bufferLevel > 0);
#ifndef WIN32
    sei();        // end critical section
#endif
    return hasEvents;
}

uint32_t EventBuffer::getBufferLevelMax() {
    // TODO : check if we need to disable/re-enable interrupts here
    uint32_t tmpLevel = bufferLevelMax;
    bufferLevelMax    = 0;        // reset level when after reading it
    return tmpLevel;
}
