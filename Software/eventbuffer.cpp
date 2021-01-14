#include "eventbuffer.h"
#include "logging.h"

extern uLog theLog;

eventBuffer::eventBuffer() {}

void eventBuffer::pushEvent(event theEvent) {
    if (event::none != theEvent) {
        theLog.snprintf(loggingLevel::Debug, "push event : %s", toString(theEvent));

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
    if (level > 0U) {
        theEvent = theEventBuffer[head];        // read the oldest event
        head     = (head + 1U) % length;        // advance readIndex to next position
        level--;                                // adjust level to one item less
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

const char* eventBuffer::toString(event anEvent) {
    switch (anEvent) {
        case event::none:
            return "none";
            break;
        case event::emergencyStopButtonPressed:
            return "emergencyStop button pressed";
            break;
        case event::emergencyStopButtonReleased:
            return "emergencyStop button released";
            break;
        case event::feedHoldResumeButtonPressed:
            return "feed Hold/Resume button pressed";
            break;
        case event::feedHoldResumeButtonReleased:
            return "feed Hold/Resume button released";
            break;
        case event::probeSwitchClosed:
            return "probeSwitch closed";
            break;
        case event::probeSwitchOpened:
            return "probeSwitch opened";
            break;
        case event::limitSwitchXMinClosed:
            return "limitSwitch XMin closed";
            break;
        case event::limitSwitchYMinClosed:
            return "limitSwitch YMin closed";
            break;
        case event::limitSwitchZMinClosed:
            return "limitSwitch ZMin closed";
            break;
        case event::limitSwitchXMaxClosed:
            return "limitSwitch XMax closed";
            break;
        case event::limitSwitchYMaxClosed:
            return "limitSwitch YMax closed";
            break;
        case event::limitSwitchZMaxClosed:
            return "limitSwitch ZMax closed";
            break;
        case event::limitSwitchXMinOpened:
            return "limitSwitch XMin opened";
            break;
        case event::limitSwitchYMinOpened:
            return "limitSwitch YMin opened";
            break;
        case event::limitSwitchZMinOpened:
            return "limitSwitch ZMin opened";
            break;
        case event::limitSwitchXMaxOpened:
            return "limitSwitch XMax opened";
            break;
        case event::limitSwitchYMaxOpened:
            return "limitSwitch YMax opened";
            break;
        case event::limitSwitchZMaxOpened:
            return "limitSwitch ZMax opened";
            break;
        case event::motionAdded:
            return "motion added";
            break;
        case event::motionCompleted:
            return "motion completed";
            break;
        case event::allMotionsCompleted:
            return "all motions completed";
            break;
        case event::motionStarted:
            return "motion started";
            break;
        case event::motionStopped:
            return "motion stopped";
            break;
        case event::motionBufferOverflow:
            return "motionBufferOverflow";
            break;
        default:
            return "unknown";
            break;
    }
}
