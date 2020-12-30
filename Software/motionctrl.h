#pragma once

// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>

#endif

#include "eventbuffer.h"              // needed for definition of eventBuffer : Motion can signal events back to the mainController
#include "gcodeparseresult.h"         // motionControl received a gCodeParseResult parameter in append()
#include "machineproperties.h"        //
#include "motionbuffer.h"             // motionControl contains a motionBuffer
#include "step.h"                     // needed for definition of step, this is an output calculated by Motion
#include "stepbuffer.h"               //
#include "stepsignals.h"              //

class motionCtrl {
  public:
    motionCtrl(eventBuffer &theEventBuffer, machineProperties &theMachineProperties, overrides &theOverrides, stepBuffer &theStepBuffer);
    void run();                                                                        //
    void optimize();                                                                   //
    void optimizePair(int32_t junctionIndex);                                          //
    void append(gCodeParserResult &theParseResult);                                    //

    float vJunction(uint32_t left, uint32_t right) const;                              //
    motionStrategy strategy() const;                                                   //
    float sampleTime() const;                                                          //
    step nextStep();                                                                   //
    bool needStepForward(uint8_t axis);                                                //
    bool needStepBackward(uint8_t axis);                                               //
    void calcNextPositionInMm(uint8_t axis, float sNow, motion *currentMotion);        //
    bool isTimedOut();                                                                 //
    step output(uint32_t timeBefore);                                                  //

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    motionBuffer theMotionBuffer;                   // instance of the object
    eventBuffer &theEventBuffer;                    // reference to the eventBuffer of the parent mainController, so this class can push events there
    machineProperties &theMachineProperties;        // reference to all the pysical properties of the machine - to be read frm .cfg file
    overrides &theOverrides;                        // reference to override settings for feedrate and spindle-rpm
    stepBuffer &theStepBuffer;
    stepSignals theStepSignals;

    motionState theState = motionState::ready;

    bool isOptimal{false};
    uint32_t sampleIndex{0};                                                    // zero based number of the sample
    float sampleZeroOffset{0.0F};                                               // time [s] between start of motion and first (index == 0) sample
    uint32_t ticksSinceLastOutput{0};                                           //
    int32_t currentPositionInSteps[(uint8_t)axis::nmbrAxis] = {0, 0, 0};        // CAUTION, signed int!, could go negative during homing etc..
    float nextPositionInMm[(uint8_t)axis::nmbrAxis]         = {0, 0, 0};        // TODO : how to deal with intialization if number of axis is different
};
