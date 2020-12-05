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
    bufferLevel       = 0;
    bufferHead        = 6;
    bufferTimeInTicks = 0;

    while (needsFilling()) {                                         // this buffer should never be empty, so let's add some minimal items to it
        write(step{defaultReloadTime, defaultOutputSignals});        //
    }

    theLog.output(loggingLevel::Debug, "stepBuffer initialized");
}

void stepBuffer::write(step aStep) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    if (bufferLevel < bufferLength) {
        uint32_t writeIndex;
        writeIndex                    = (bufferLength + bufferHead + bufferLevel - 2) % bufferLength;        // TODO : explain
        buffer[writeIndex].timeBefore = aStep.timeBefore;                                                    //
        writeIndex                    = (writeIndex + 2) % bufferLength;                                     //
        buffer[writeIndex].signals    = aStep.signals;                                                       //
        bufferLevel++;                                                                                       //
        bufferTimeInTicks += aStep.timeBefore;
    } else {
        theLog.snprintf(loggingLevel::Critical, "stepBufferOverflow level = %d, time = %d", bufferLevel, bufferTimeInTicks);
        for (uint32_t i = 0; i < bufferLength; i++) {
            theLog.snprintf(loggingLevel::Debug, "item %d time = %d signals = %d", i, buffer[i].timeBefore, buffer[i].signals);
        }
    }
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
}

step stepBuffer::read() {
    if (bufferLevel > 0) {
        step aStep;
        aStep.timeBefore = buffer[bufferHead].timeBefore;
        aStep.signals    = buffer[bufferHead].signals;
        bufferTimeInTicks -= buffer[bufferHead].timeBefore;
        bufferHead = (bufferHead + 1) % bufferLength;
        bufferLevel--;
        return aStep;
    } else {
        theLog.output(loggingLevel::Critical, "stepBufferUnderRun");
        return (step{defaultReloadTime, defaultOutputSignals});
    }
}

uint32_t stepBuffer::getBufferTimeInTicks() const {
    return bufferTimeInTicks;
}

uint32_t stepBuffer::getBufferLevel() const {
    return bufferLevel;
}

bool stepBuffer::needsFilling() const {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    noInterrupts();
#endif
    bool result = ((bufferLevel < minBufferLevel) || (bufferTimeInTicks < minStepBufferTotalTimeTicks));
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    interrupts();
#endif
    return result;
}
