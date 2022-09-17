// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class modalGroup : uint32_t
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
    PathControlMode,
    // ArcDistance,
    // CutterCompensation,
    // ToolLengthOffset,
    // CannedCycles
    // Coolant
    // ProgramFlow
    nmbrModalGroups
    };
constexpr uint32_t nmbrModalGroups = static_cast<uint32_t>(modalGroup::nmbrModalGroups);

enum class modalGroup0 : uint32_t
    {
    G4, G10, G28, G30, G53,
    //  G92, G92_1, G92_2, G92_3
    nmbrModes
    };

enum class modalGroupMotion : uint32_t
    {
    G0, G1, G2, G3,
    nmbrModes
    };

enum class modalGroupFeedRate : uint32_t
    {
    G94, G93,
    nmbrModes
    };

enum class modalGroupSpindle : uint32_t
    {
    M5, M3, M4,
    nmbrModes
    };

enum class modalGroupUnit : uint32_t
    {
    G21, G20,
    nmbrModes
    };

enum class modalGroupPlane : uint32_t
    {
    G17, G18, G19,
    nmbrModes
    };

enum class modalGroupDistance : uint32_t
    {
    G90, G91,
    nmbrModes
    };

enum class modalGroupCoordinateSet : uint32_t
    {
    G53, G54, G55, G56, G57, G58, G59, G59_1, G59_2, G59_3,
    nmbrCoordinateSets
    };
constexpr uint32_t nmbrCoordinateSets = static_cast<uint32_t>(modalGroupCoordinateSet::nmbrCoordinateSets);

enum class modalGroupOverrides : uint32_t
    {
    M48, M49,
    nmbrModes
    };

enum class modalGroupCoolantFlood : uint32_t
    {
    M9, M7,
    nmbrModes
    };

enum class modalGroupCoolantMist : uint32_t
    {
    M9, M8,
    nmbrModes
    };

enum class modalGroupPathControlMode : uint32_t
    {
    exactPath, continuous,
    nmbrModes
    };

