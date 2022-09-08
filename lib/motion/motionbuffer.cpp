// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motionbuffer.h"

bool motionBuffer::isEmpty() const {
    return (0 == level);
}

bool motionBuffer::isFull() const {
    return (level >= length);
}

uint32_t motionBuffer::push() {
    uint32_t writeIndex = ((head + level) % length);
    if (level >= length) {
        lastError = event::motionBufferOverflow;
    } else {
        level++;
    }
    if (level > maxLevel) {
        maxLevel = level;
    }
    return writeIndex;
}

void motionBuffer::pop() {
    head = (head + 1) % length;
    if (level > 0) {
        --level;
    } else {
        lastError = event::motionBufferUnderflow;
    }
}

motion& motionBuffer::getHead() {
    return motionBuffer[head];
}

uint32_t motionBuffer::getLevel() const {
    return level;
}

uint32_t motionBuffer::getMaxLevel() {
    uint32_t result = maxLevel;
    maxLevel        = 0;
    return result;
}

void motionBuffer::flush() {
    head  = 0;
    level = 0;
}

event motionBuffer::getLastError() {
    event result = lastError;
    lastError    = event::none;
    return result;
}
