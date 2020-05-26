// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motionbuffer.h"

bool MotionBuffer::isEmpty() const
    {
    return (0 == bufferLevel);
    }

bool MotionBuffer::isFull() const
    {
    return (bufferLevel >= bufferLength);
    }

uint32_t MotionBuffer::push()
    {
	// TODO : think about what to do when buffer is already full
	uint32_t writeIndex = ((readIndex + bufferLevel) % bufferLength);		// first free item at tail of motionBuffer
    bufferLevel++;															// adjust level, as we have one more item now
    return writeIndex;
    }

void MotionBuffer::pop()
    {
	// TODO : think about what to do when buffer is empty
	readIndex = (readIndex + 1) % bufferLength;
    --bufferLevel;
    }