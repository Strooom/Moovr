// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motionbuffer.h"
#ifdef WIN32
#include <fstream>
#include <iostream>
#endif

bool motionBuffer::isEmpty() const {
    return (0 == bufferLevel);
}

bool motionBuffer::isFull() const {
    return (bufferLevel >= bufferLength);
}

uint32_t motionBuffer::push() {
    // TODO : think about what to do when buffer is already full
    uint32_t writeIndex = ((readIndex + bufferLevel) % bufferLength);        // first free item at tail of motionBuffer
    bufferLevel++;                                                           // adjust level, as we have one more item now
    if (bufferLevel > bufferLevelMax) {
        bufferLevelMax = bufferLevel;
    }
    return writeIndex;
}

void motionBuffer::pop() {
    // TODO : think about what to do when buffer is empty
    readIndex = (readIndex + 1) % bufferLength;
    --bufferLevel;
}

motion* motionBuffer::current() {
    return &motionBuffer[readIndex];
}

uint32_t motionBuffer::level() {
    return bufferLevel;
}

void motionBuffer::export2csv(const char* outputFilename) {
#ifdef WIN32

    std::ofstream outputFile(outputFilename);
    for (uint32_t i = 0; i < bufferLevel; i++) {
        for (uint32_t j = 0; j < 100; j++) {
            float t = (motionBuffer[(readIndex + i) % bufferLength].speedProfile.duration * static_cast<float>(j)) / static_cast<float>(100);
            outputFile << motionBuffer[(readIndex + i) % bufferLength].v(t) << std::endl;
        }
    }
    outputFile.close();
#endif
}
