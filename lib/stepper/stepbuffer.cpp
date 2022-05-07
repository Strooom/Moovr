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

void noInterrupts();        // forward declaration - function is defined in some HAL or a dummy in the unitTest application
void interrupts();          // idem

stepBuffer::stepBuffer() {
    initialize();
}

void stepBuffer::initialize() {
    level       = 0;
    head        = 0;
    timeInTicks = 0;

    while (needsFilling()) {                                                   // this buffer should never be empty, so let's add some minimal items to it
        write(step{(minStepBufferTotalTimeTicks / minBufferLevel), 0});        //
    }
}

void stepBuffer::write(step aStep) {
    noInterrupts();
    if (level < length) {
        uint32_t writeIndex;
        writeIndex                    = (length + head + level - 2) % length;        // TODO : explain
        buffer[writeIndex].timeBefore = aStep.timeBefore;                            //
        writeIndex                    = (writeIndex + 2) % length;                   //
        buffer[writeIndex].signals    = aStep.signals;                               //
        level++;                                                                     //
        timeInTicks += aStep.timeBefore;
    } else {
    }
    interrupts();
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
        return (step{1000, 1000});        // Todo - fix these values
    }
}

uint32_t stepBuffer::getTimeInTicks() const {
    return timeInTicks;
}

uint32_t stepBuffer::getLevel() const {
    return level;
}

// bool stepBuffer::isFull() const {
//     noInterrupts();
//     bool result = (level < length);
//     interrupts();
//     return result;
// }

// bool stepBuffer::isEmpty() const {
//     return (level == 0);
// }


bool stepBuffer::needsFilling() const {
    noInterrupts();
    bool result = ((level < minBufferLevel) || (timeInTicks < minStepBufferTotalTimeTicks));
    interrupts();
    return result;
}
