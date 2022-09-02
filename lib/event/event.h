// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class event : uint32_t {
    none,
    // buttons
    emergencyStopButtonPressed,
    emergencyStopButtonReleased,
    feedHoldResumeButtonPressed,
    feedHoldResumeButtonReleased,
    // switches
    probeSwitchClosed,
    probeSwitchOpened,
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
    // stepBuffer
    stepBufferUnderflow,
    stepBufferOverflow,
    // motionBuffer
    motionAdded,
    motionCompleted,
    allMotionsCompleted,
    motionStarted,
    motionStopped,
    motionBufferOverflow,
    // homing
    homingStarted,
    homingCompleted,
    homingError,
    // probing
    probingStarted,
    probingCompleted,

    // eventBuffer
    eventBufferUnderflow,
    eventBufferOverflow
};

const char* toString(event anEvent);