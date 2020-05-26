// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#ifndef  WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

enum class gCodeLetter : uint8_t
    {
    //	All AXIS letters are put at the top, in the SAME order as in 'axis'. This way we can use loops to copy gCode letterValues to axis..
    X, Y, Z,
    //	A, B, C,
    //	D, E,
    //	U, V, W,

    //	After the AXI letters, I put the OFFSET letters, so we can easily loop over those as well..
    I, J, K,
    L,
    F,
    //	G, H,
    //	M,
    N,
    //	O,
    P,
    //	Q,
    R, S, T,
    nmbrLetters
    };
constexpr uint32_t nmbrLetters = static_cast<uint32_t>(gCodeLetter::nmbrLetters);

enum class modalGroup : uint8_t
    {
    Group0,
    Motion,
    Plane,
    Distance,
    Spindle,
    Unit,
    FeedRate,
    CoordinateSet,
    CoolantFlood,
    CoolantMist,
    ManualOverride,
    // ArcDistance,
    // CutterCompensation,
    // ToolLengthOffset,
    // CannedCycles
    // Coolant
    // ProgramFlow
    nmbrModalGroups
    };
constexpr uint32_t nmbrModalGroups = static_cast<uint32_t>(modalGroup::nmbrModalGroups);

enum class modalGroup0 : uint8_t
    {
    G4, G10, G28, G30, G53,
    //  G92, G92_1, G92_2, G92_3
    nmbrModes
    };

enum class modalGroupMotion : uint8_t
    {
    G0, G1, G2, G3,
    nmbrModes
    };

enum class modalGroupFeedRate : uint8_t
    {
    G94, G93,
    nmbrModes
    };

enum class modalGroupSpindle : uint8_t
    {
    M5, M3, M4,
    nmbrModes
    };

enum class modalGroupUnit : uint8_t
    {
    G21, G20,
    nmbrModes
    };

enum class modalGroupPlane : uint8_t
    {
    G17, G18, G19,
    nmbrModes
    };

enum class modalGroupDistance : uint8_t
    {
    G90, G91,
    nmbrModes
    };

enum class modalGroupCoordinateSet : uint8_t
    {
    G53, G54, G55, G56, G57, G58, G59, G59_1, G59_2, G59_3,
    nmbrCoordinateSets
    };
constexpr uint32_t nmbrCoordinateSets = static_cast<uint32_t>(modalGroupCoordinateSet::nmbrCoordinateSets);

enum class modalGroupOverrides : uint8_t
    {
    M48, M49,
    nmbrModes
    };

enum class modalGroupCoolantFlood : uint8_t
    {
    M9, M7,
    nmbrModes
    };

enum class modalGroupCoolantMist : uint8_t
    {
    M9, M8,
    nmbrModes
    };
