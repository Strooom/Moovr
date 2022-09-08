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


stepBuffer::stepBuffer(uint32_t aMinimumTotalTime, uint32_t aMinimumLevel) : lowWaterTotalTime(aMinimumTotalTime), lowWaterMark(aMinimumLevel) {
    initialize();
}

void stepBuffer::initialize() {
    level     = 0;
    head      = 0;
    totalTime = 0;

    while (needsFilling()) {                                      // this buffer should never be empty, so let's add some minimal items to it
        write(step{(lowWaterTotalTime / lowWaterMark), 0});        // TODO 3 : prevent divide by zero
    }
}

void stepBuffer::write(step aStep) {
    if (level < length) {
        uint32_t writeIndexSignals              = (head + level) % length;                     // signals is written at the usual position
        uint32_t writeIndexTimeBefore           = (length + head + level - 2) % length;        // timeBefore is written 2 positions back
        buffer[writeIndexTimeBefore].timeBefore = aStep.timeBefore;                            //
        buffer[writeIndexSignals].signals       = aStep.signals;                               //
        level++; // add maxLevel tracking
        totalTime += aStep.timeBefore;
    } else {
        lastError = event::stepBufferOverflow; // TODO 2 : critical problem, resulting in crash of the buffer output...
    }
}

step stepBuffer::read() {
    if (level > 0) {
        step aStep;
        aStep.timeBefore = buffer[head].timeBefore;
        aStep.signals    = buffer[head].signals;
        totalTime -= buffer[head].timeBefore;
        head = (head + 1) % length;
        level--; // add minLevel tracking
        return aStep;
    } else {
        lastError = event::stepBufferUnderflow;
        return (step{1000, 1000});        // TODO 4 - fix these values
    }
}

uint32_t stepBuffer::getTimeInTicks() const {
    return totalTime;
}

uint32_t stepBuffer::getLevel() const {
    return level;
}

bool stepBuffer::needsFilling() const {
    bool result = ((level < lowWaterMark) || (totalTime < lowWaterTotalTime)); // TODO 1 : prevent overflow
    return result;
}

event stepBuffer::getLastError() {
    event result = lastError;
    lastError    = event::none;
    return result;
}

uint32_t stepBuffer::getMinLevel() {
    uint32_t result = minLevel;
    minLevel = length;
    return result;
}

uint32_t stepBuffer::getMaxLevel() {
    uint32_t result = maxLevel;
    minLevel = 0;
    return result;
}
