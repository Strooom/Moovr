// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// Note about thread-safety : this buffer will be written and read by two different threads :
// written by main program
// read by timer interrupt handler
// so in order to make this threadsafe, disable interrupts to avoid collision with read

#include "stepbuffer.h"


stepBuffer::stepBuffer(uint32_t aMinimumTotalTime, uint32_t aMinimumLevel) : minimumTotalTime(aMinimumTotalTime), minimumLevel(aMinimumLevel) {
    initialize();
}

void stepBuffer::initialize() {
    level     = 0;
    head      = 0;
    totalTime = 0;

    while (needsFilling()) {                                      // this buffer should never be empty, so let's add some minimal items to it
        write(step{(minimumTotalTime / minimumLevel), 0});        //
    }
}

void stepBuffer::write(step aStep) {
    if (level < length) {
        uint32_t writeIndexSignals              = (head + level) % length;                     // signals is written at the usual position
        uint32_t writeIndexTimeBefore           = (length + head + level - 2) % length;        // timeBefore is written 2 positions back
        buffer[writeIndexTimeBefore].timeBefore = aStep.timeBefore;                            //
        buffer[writeIndexSignals].signals       = aStep.signals;                               //
        level++;
        totalTime += aStep.timeBefore;
    } else {
        lastError = event::stepBufferOverflow; // critical problem, resulting in crash of the buffer output...
    }
}

step stepBuffer::read() {
    if (level > 0) {
        step aStep;
        aStep.timeBefore = buffer[head].timeBefore;
        aStep.signals    = buffer[head].signals;
        totalTime -= buffer[head].timeBefore;
        head = (head + 1) % length;
        level--;
        return aStep;
    } else {
        lastError = event::stepBufferUnderflow;
        return (step{1000, 1000});        // TODO - fix these values
    }
}

uint32_t stepBuffer::getTimeInTicks() const {
    return totalTime;
}

uint32_t stepBuffer::getLevel() const {
    return level;
}

bool stepBuffer::needsFilling() const {
    bool result = ((level < minimumLevel) || (totalTime < minimumTotalTime));
    return result;
}

event stepBuffer::getLastError() {
    event result = lastError;
    lastError    = event::none;
    return result;
}
