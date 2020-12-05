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

#include "eventBuffer.h"             // needed for definition of eventBuffer : Motion can signal events back to the mainController
#include "gcodeparseresult.h"        // motionControl received a gCodeParseResult parameter in append()
#include "machineproperties.h"
#include "motionbuffer.h"        // motionControl contains a motionBuffer
#include "step.h"                // needed for definition of step, this is an output calculated by Motion
#include "stepbuffer.h"

//#include "general.h"                  // needed for definition of eg. axis
//#include "machineproperties.h"        // needed for definition of machineProperties : this is passed to Motion so it can include machine limitations eg. during speed calculations

class motionCtrl {
  public:
    motionCtrl(eventBuffer &theEventBuffer, machineProperties &theMachineProperties, overrides &theOverrides, stepBuffer &theStepBuffer);
    void run();                                            //
    void optimize();                                       //
    void append(gCodeParserResult &theParseResult);        //
    step getNextStep();                                    //

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    motionBuffer theMotionBuffer;                   // instance of the object
    eventBuffer &theEventBuffer;                    // reference to the eventBuffer of the parent mainController, so this class can push events there
    machineProperties &theMachineProperties;        // reference to all the pysical properties of the machine - to be read frm .cfg file
    overrides &theOverrides;                        // reference to override settings for feedrate and spindle-rpm
    stepBuffer &theStepBuffer;

    // StateMachine
    MotionState theMotionState = MotionState::ready;
    MotionStrategy theStrategy = MotionStrategy::maximizeSpeed;        // should the machine maximize or minimze speed ?

    uint32_t timeInMotionTicks                              = 0;                // time elapsed in this motion, in discrete, integer timer ticks
    int32_t currentPositionInSteps[(uint8_t)axis::nmbrAxis] = {0, 0, 0};        // CAUTION, signed int!, could go negative during homing etc..
    float nextPositionInMm[(uint8_t)axis::nmbrAxis];                            // ???
};
