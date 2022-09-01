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
    // TODO : think about what to do when buffer is already full
    uint32_t writeIndex = ((head + level) % length);        // first free item at tail of motionBuffer
    level++;                                                // adjust level, as we have one more item now
    if (level > levelMax) {
        levelMax = level;
    }
    return writeIndex;
}

void motionBuffer::pop() {
    // TODO : think about what to do when buffer is empty
    head = (head + 1) % length;
    --level;
}

motion& motionBuffer::getHead() {
    return motionBuffer[head];
}

uint32_t motionBuffer::getLevel() const {
    return level;
}

void motionBuffer::flush() {
    head  = 0;
    level = 0;
}

// void motionBuffer::export2csv(const char* outputFilename) {
// #ifdef WIN32

//     std::ofstream outputFile(outputFilename);
//     for (uint32_t i = 0; i < level; i++) {
//         for (uint32_t j = 0; j < 100; j++) {
//             float t = (motionBuffer[(head + i) % length].speedProfile.duration * static_cast<float>(j)) / static_cast<float>(100);
//             outputFile << motionBuffer[(head + i) % length].v(t) << std::endl;
//         }
//     }
//     outputFile.close();
// #endif
//}
