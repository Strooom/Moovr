#pragma once

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

enum class event : uint8_t {
    none,
    emergencyStopButtonPressed,
    emergencyStopButtonReleased,
    feedHoldResumeButtonPressed,
    feedHoldResumeButtonReleased,
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
    motionAdded,
    motionCompleted,
    allMotionsCompleted,
    motionStarted,
    motionStopped,
    motionBufferOverflow
};
