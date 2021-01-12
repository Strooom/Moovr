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
#include "sampletime.h"
#include "step.h"               // needed for definition of step, this is an output calculated by Motion
#include "stepbuffer.h"         //
#include "stepsignals.h"        //

class motionCtrl {
  public:
    motionCtrl(eventBuffer &anEventBuffer, machineProperties &someMachineProperties, overrides &someOverrides, stepBuffer &aStepBuffer);
    void run();                                                                          //
    void optimize();                                                                     //
    void optimizePair(int32_t junctionIndex);                                            //
    void append(gCodeParserResult &aParseResult);                                        //
    float vJunction(uint32_t left, uint32_t right) const;                                //
    motionStrategy strategy() const;                                                     //
    step nextStep();                                                                     //
    bool needStepForward(uint8_t anAxis);                                                //
    bool needStepBackward(uint8_t anAxis);                                               //
    void calcNextPositionInMm(uint8_t anAxis, float sNow, motion *currentMotion);        //
    motionState theState = motionState::ready;
    bool isRunning() const;
    bool isStopped() const;
    void calcStepSignals();
    void stop();
    void next();
    motionBuffer theMotionBuffer;                                  // instance of the object


#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    machineProperties &theMachineProperties;        // reference to all the pysical properties of the machine - to be read frm .cfg file
    eventBuffer &theEventBuffer;                    // reference to the eventBuffer of the parent mainController, so this class can push events there
    overrides &theOverrides;                        // reference to override settings for feedrate and spindle-rpm
    stepBuffer &theStepBuffer;

    stepSignals theStepSignals;                                    //
    sampleTime theSampleTime = sampleTime(theMotionBuffer);        //

    bool isOptimal{false};
    int32_t currentPositionInSteps[(uint8_t)axis::nmbrAxis]{};        // CAUTION, signed int!, could go negative during homing etc..
    float nextPositionInMm[(uint8_t)axis::nmbrAxis]{};                // TODO : how to deal with intialization if number of axis is different
};
