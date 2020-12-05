#pragma once


#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
#include <Arduino.h>
static constexpr uint32_t busFrequency = F_BUS;             // Clock going into the Programmable Interval Timer (PIT2)
#else
#include <stdint.h>        // access to uintxx_t data types
static constexpr uint32_t busFrequency = 60'000'000U;
#endif

static constexpr uint32_t outputTimerFrequency        = 100'000U;                                             // Design parameter resulting in a certain maximum stepping frequency, as well as a minimum Step pulse width and Dir setup timeBefore
static constexpr uint32_t maxSteppingFrequency        = outputTimerFrequency / 2;                             //
static constexpr float minStepPulseWidth              = 1.0F / outputTimerFrequency;                          // period of the PIT1 timer, this will automatically become the DIR setup timeBefore, as well as the STEP pulse width
static constexpr float minStepBufferTotalTime         = 0.01F;                                                // amount of totalTime [ms] we want at least in the buffer
static constexpr uint32_t minStepBufferTotalTimeTicks = outputTimerFrequency * minStepBufferTotalTime;        // amount of totalTime [PIT1 ticks] we want at least in the buffer

// TODO : this also results in a maximum timeBefore between to steps = minimum stepping frequency
// TODO : this also results in a maximum total timeBefore for one motion...

static constexpr uint32_t inputTimerFrequency = 100U;                                      // Design parameter : 100 Hz = 10ms
static constexpr uint32_t inputSamplingRate   = inputTimerFrequency;                       //
static constexpr uint32_t pit2Reload          = busFrequency / inputTimerFrequency;        //

// TODO : these may not belong here.. Find a better location
static constexpr float hysteresis = 0.05f;        // hysteresis, to avoid setting steps forward and backwards due to floating point rounding errors. In fact the value put here is half of the hysteresis
