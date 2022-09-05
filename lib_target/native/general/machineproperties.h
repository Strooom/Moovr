// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <limits>
#include <stdint.h>
#include "axis.h"

static constexpr uint32_t busFrequency = 60'000'000U;

static constexpr uint32_t nmbrInputs = 9U;
static constexpr uint32_t inputSamplingInterval{10U};        // [ms] interval between sampling inputs like buttons and limitswitches

static constexpr uint32_t outputTimerFrequency     = 60'000U;                                                        // Design parameter resulting in a certain maximum stepping frequency, as well as a minimum Step pulse width and Dir setup timeBefore
static constexpr uint32_t maxSteppingFrequency     = outputTimerFrequency / 2U;                                      //
static constexpr float minStepPulseWidth           = 1.0F / outputTimerFrequency;                                    // period of the PIT1 timer, this will automatically become the DIR setup timeBefore, as well as the STEP pulse width
static constexpr float minStepBufferTotalTime      = 0.025F;                                                         // amount of totalTime [s] we want at least in the buffer
static constexpr uint32_t minStepBufferTotalTimeMs = static_cast<uint32_t>(minStepBufferTotalTime * 1000.0F);        // amount of totalTime [ms] we want at least in the buffer

static constexpr uint32_t minStepBufferTotalTimeTicks = static_cast<uint32_t>(outputTimerFrequency * minStepBufferTotalTime);                                       // amount of totalTime [PIT1 ticks] we want at least in the buffer
static constexpr uint32_t minStepBufferLevel          = 4U;                                                                                                         //
static constexpr uint32_t maxTicksSinceLastOutput     = minStepBufferTotalTimeTicks / minStepBufferLevel;                                                           //
static constexpr float minSteppingFrequency           = static_cast<float>(maxSteppingFrequency) / static_cast<float>(std::numeric_limits<uint32_t>::max());        //
static constexpr float maxMotionDuration              = static_cast<float>(std::numeric_limits<uint32_t>::max() / outputTimerFrequency);                            //
static constexpr float hysteresis                     = 0.05F;                                                                                                      // hysteresis, to avoid setting steps forward and backwards due to floating point rounding errors. In fact the value put here is half of the hysteresis

static constexpr float oneSixth{1.0F / 6.0F};        // constant to avoid having to divide by 6, as division is slower than multiplication

class machineProperties {
  public:
    void load();
    void save();
    void setForTest(uint32_t set);

    struct Motors {
        float jMax{1000.0F};                                    // [mm/s^3] // TODO should we not set jMax per axis ?
        float aMax[nmbrAxis]{200.0F, 200.0F, 200.0F};           // [mm/s^2]
        float dMax[nmbrAxis]{-200.0F, -200.0F, -200.0F};        // [mm/s^2]
        float vMax[nmbrAxis]{200.0F, 200.0F, 200.0F};           // [mm/s]
        float sMax[nmbrAxis]{600.0F, 800.0F, 130.0F};           // [mm]
        float stepsPerMm[nmbrAxis]{50.0F, 50.0F, 50.0F};        // [1/mm]
    } motors;

    struct Limits {
        bool hasLimitsMax[nmbrAxis]{true, false, true};          // limit switches towards the positive direction of the Axis
        uint32_t limitMaxIndex[nmbrAxis]{1, 2, 0};               // index into myInputs[] telling which input is the matching limit switch
        bool hasLimitsMin[nmbrAxis]{false, false, false};        // limit switches towards the negative direction of the Axis
        uint32_t limitMinIndex[nmbrAxis]{0, 2, 4};               // index into myInputs[] telling which input is the matching limit switch
        float maxLimitswitchTravel{2.0F};                        // [mm]
    } limits;

    struct Spindle {
        bool hasVariableSpindle{false};
        bool hasSpindledirection{false};
        float minSpindleRPM{10000.0F};
        float maxSpindleRPM{30000.0F};
    } spindle;

    struct Coolant {
        bool hasCoolantFlood{true};
        bool hasCoolantMist{true};
    } coolant;

    double minLengthSProfile{0.0F};                                              // [mm] all motions with a length smaller will be 2nd order T-profile - larger will be 3rd order S-profile
    float vMaxHoming{motors.jMax * motors.jMax * motors.jMax * oneSixth};        //

    float vHoming{15};           // faster homing speed, towards switch closing
    float vHomingSlow{1};        // slower homing, towards opening limitswitch

    axis homingSequence[nmbrAxis]{axis::Z, axis::X, axis::nmbrAxis};        // in which sequence do we want to home axis.
    bool homingDirection[nmbrAxis]{true, true, true};                       // in which direction do we want to home : true = positive, false = negative
    double homingOffset[nmbrAxis]{-10.0, -10.0, -10.0};                     // after homing, what distance from the limitswitches do we set the machine zero
};
