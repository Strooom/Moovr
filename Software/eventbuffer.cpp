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

eventBuffer::eventBuffer() {}

void eventBuffer::pushEvent(event theEvent) {
    if (event::none != theEvent) {
        theLog.snprintf(loggingLevel::Debug, "push event : %s", toString(theEvent));

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        noInterrupts();
#endif
        if (bufferLevel < bufferLength) {
            theEventBuffer[(bufferReadIndex + bufferLevel) % bufferLength] = theEvent;        // write new event at next writeIndex = readIndex + Level
            bufferLevel++;                                                                    // adjust level to one item more
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
    if (bufferLevel > 0U) {
        theEvent        = theEventBuffer[bufferReadIndex];              // read the oldest event
        bufferReadIndex = (bufferReadIndex + 1U) % bufferLength;        // advance readIndex to next position
        bufferLevel--;                                                  // adjust level to one item less
    } else {
        theLog.output(loggingLevel::Error, "Eventbuffer Underflow");
    }
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    theLog.snprintf(loggingLevel::Debug, "pop event : %s", toString(theEvent));
    return theEvent;
}

bool eventBuffer::hasEvents() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    bool hasEvents = (bufferLevel > 0U);
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    return hasEvents;
}

uint32_t eventBuffer::getBufferLevelMax() {
    // TODO : check if we need to disable/re-enable interrupts here
    uint32_t tmpLevel = bufferLevelMax;        //
    bufferLevelMax    = 0U;                    // reset level when after reading it
    return tmpLevel;
}


const char * eventBuffer::toString(event anEvent) const {
    switch (anEvent) {
        case event::none:
            return "none";
            break;
        case event::emergencyStopButtonPressed:
            return "";
            break;
        case event::emergencyStopButtonReleased:
            break;
        case event::feedHoldResumeButtonPressed:
            return "feed Hold/Resume button pressed";
            break;
        case event::feedHoldResumeButtonReleased:
            return "feed Hold/Resume button released";
            break;
        case event::probeSwitchClosed:
            return "";
            break;
        case event::probeSwitchOpened:
            return "";
            break;
        case event::limitSwitchXMinClosed:
            return "";
            break;
        case event::limitSwitchYMinClosed:
            return "";
            break;
        case event::limitSwitchZMinClosed:
            return "";
            break;
        case event::limitSwitchXMaxClosed:
            return "";
            break;
        case event::limitSwitchYMaxClosed:
            return "";
            break;
        case event::limitSwitchZMaxClosed:
            return "";
            break;
        case event::limitSwitchXMinOpened:
            return "";
            break;
        case event::limitSwitchYMinOpened:
            return "";
            break;
        case event::limitSwitchZMinOpened:
            return "";
            break;
        case event::limitSwitchXMaxOpened:
            return "";
            break;
        case event::limitSwitchYMaxOpened:
            return "";
            break;
        case event::limitSwitchZMaxOpened:
            return "";
            break;
        case event::motionAdded:
            return "";
            break;
        case event::motionCompleted:
            return "motion completed";
            break;
        case event::allMotionsCompleted:
            return "all motions completed";
            break;
        case event::motionStarted:
            return "";
            break;
        case event::motionStopped:
            return "motion stopped";
            break;
        case event::motionBufferOverflow:
            return "";
            break;
        default:
            break;
    }
}
