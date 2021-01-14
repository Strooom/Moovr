// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// Note : in order to make this class threadsafe, initialize and fill have to disable interrupts to avoid collision with read

#include "stepbuffer.h"
#include "logging.h"

extern uLog theLog;

stepBuffer::stepBuffer() {
    initialize();
}

void stepBuffer::initialize() {
    level       = 0;
    head        = 6;
    timeInTicks = 0;

    while (needsFilling()) {                                         // this buffer should never be empty, so let's add some minimal items to it
        write(step{250, 0});        // TODO : fix these values
    }

    theLog.output(loggingLevel::Debug, "stepBuffer initialized");
}

void stepBuffer::write(step aStep) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    if (level < length) {
        uint32_t writeIndex;
        writeIndex                    = (length + head + level - 2) % length;        // TODO : explain
        buffer[writeIndex].timeBefore = aStep.timeBefore;                                                    //
        writeIndex                    = (writeIndex + 2) % length;                                     //
        buffer[writeIndex].signals    = aStep.signals;                                                       //
        level++;                                                                                       //
        timeInTicks += aStep.timeBefore;
    } else {
        theLog.snprintf(loggingLevel::Critical, "stepBufferOverflow level = %d, time = %d", level, timeInTicks);
        for (uint32_t i = 0; i < length; i++) {
            theLog.snprintf(loggingLevel::Debug, "item %d time = %d signals = %d", i, buffer[i].timeBefore, buffer[i].signals);
        }
    }
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
}

step stepBuffer::read() {
    if (level > 0) {
        step aStep;
        aStep.timeBefore = buffer[head].timeBefore;
        aStep.signals    = buffer[head].signals;
        timeInTicks -= buffer[head].timeBefore;
        head = (head + 1) % length;
        level--;
        return aStep;
    } else {
        theLog.output(loggingLevel::Critical, "stepBufferUnderRun");
        return (step{1000, 1000}); // Todo - fix these values
    }
}

uint32_t stepBuffer::getTimeInTicks() const {
    return timeInTicks;
}

uint32_t stepBuffer::getLevel() const {
    return level;
}

bool stepBuffer::needsFilling() const {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    bool result = ((level < minBufferLevel) || (timeInTicks < minStepBufferTotalTimeTicks));
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    return result;
}
