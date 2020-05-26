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

enum class Event : uint8_t
    {
	none,
    emergencyStopButtonPressed,
    emergencyStopButtonReleased,
    feedHoldResumeButtonPressed,
    feedHoldResumeButtonReleased,
    probeSwitchClosed,
    probeSwitchOpenend,
    limitSwitchXMinClosed,
    limitSwitchYMinClosed,
    limitSwitchZMinClosed,
    limitSwitchXMaxClosed,
    limitSwitchYMaxClosed,
    limitSwitchZMaxClosed,
    limitSwitchXMinOpened,
    limitSwitchYMinOpened,
    limitSwitchZMinOpened,
    limitSwitchXMaxOpened,
    limitSwitchYMaxOpened,
    limitSwitchZMaxOpened,
    motionAdded,
    motionCompleted,
    allMotionsCompleted
    };

class EventBuffer
    {
    private:
        static constexpr uint8_t eventBufferLength = 32;
        Event eventBuffer[eventBufferLength];
        uint32_t eventBufferReadIndex = 0;
        uint32_t eventBufferLevel = 0;

    public:
        void pushEvent(Event);			// push an event onto the eventBuffer
        Event popEvent();				// pop an event from the eventbuffer
        bool hasEvents();				// check if there are any events
    };
