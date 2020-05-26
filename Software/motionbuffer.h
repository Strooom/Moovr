// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#ifndef  WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

#include "motionitem.h"

class MotionBuffer
    {
    public:
		static constexpr uint8_t bufferLength{ 32 };					// Length should be a compromise : large enough to allow good speed-optimization. Not too large to consume ram and cpu in optimizing
        MotionItem motionBuffer[bufferLength];							// The buffer holding all motions to be executed
        bool isEmpty() const;
        bool isFull() const;
        uint32_t push();												// pushes 1 item on the buffer and returns an index to it..
		void pop();														// pops the oldest item on from the buffer and adjusts indexes and level

    private:
        uint32_t readIndex{ 0 };										// First-to-be-executed or currently-being-executed motion
        uint32_t bufferLevel{ 0 };										// number of motions in the buffer
    };

