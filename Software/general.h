// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include <limits>

enum class axis : uint8_t {
    X,
    Y,
    Z,
    //	A, B, C,
    nmbrAxis
};
constexpr uint32_t nmbrAxis = static_cast<uint32_t>(axis::nmbrAxis);

enum class arcPlane : uint8_t {
    XY,
    YZ,
    ZX,
    nmbrArcPlanes
};

enum class rotationDirection : uint8_t {
    Clockwise,
    CounterClockwise,
    nmbrRotationDirections
};

enum class motionType : uint8_t {
    Traverse,              // 3D Linear move at maximum speed = G0
    FeedLinear,            // 3D Linear move at feedrate speed = G1
    FeedHelicalCW,         // Helical move at feedrate speed = G2
    FeedHelicalCCW,        // Helical move at feedrate speed = G3
    PauseAndResume,        // Pause for a defined time, then resume = G4
    Pause,                 // Pause for undefined time, resume on resume commmand = M0 or M1
    None,                  // When parsing a gCode block results in no motion, the motionType = None
    nmbrMotionTypes
};

enum class motionState : uint8_t {
    ready,
    running,
    stopping,
    stopped
};        // State of the MotionController

enum class motionStrategy : uint8_t {
    minimizeSpeed,
    maximizeSpeed,
    nmbrStrategies
};        // Strategy for optimizing the motion

enum class motionSpeedProfileOrder : uint8_t {
    firstOrder,
    secondOrder
};        // Shape of the speedprofile : first or second order

enum class motionCalculateDirection : uint8_t {
    forward,
    reverse
};
