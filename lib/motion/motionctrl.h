// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

#include "simplifiedmotion.h"
#include "motionbuffer.h"
#include "sampletime.h"
#include "step.h"
#include "stepsignals.h"
#include "motionstate.h"
#include "motionstrategy.h"
#include "overrides.h"

class motionCtrl {
  public:
    void append(simplifiedMotion &aSimpleMotion);
    void start();
    void hold();
    void resume();
    void stop();

    void optimize();

    motionState getState() const;
    
    // motionStrategy getMotionStrategy() const;
    // overrides getOverrides() const;
    
    bool isRunning() const;

    step calculateStepperSignals();

#ifndef unitTesting
  private:
#endif
    motionStrategy theStrategy{motionStrategy::minimizeSpeed};
    overrides theOverrides;

    sampleTime theSampleTime       = sampleTime(1);               // TODO : set a correct sample interval
    motionState theMotionCtrlState = motionState::stopped;        //
    motionBuffer theMotionBuffer;                                 //
    stepSignals theStepSignals;                                   //

    float vJunction(uint32_t left, uint32_t right) const;                                //
    void calcStepSignals();                                                              //
    void calcNextPositionInMm(uint8_t anAxis, float sNow, motion *currentMotion);        //
    bool needStepForward(uint8_t anAxis);                                                //
    bool needStepBackward(uint8_t anAxis);                                               //
    void optimizePair(int32_t junctionIndex);                                            //

    bool isOptimal{false};
    int32_t currentPositionInSteps[(uint8_t)axis::nmbrAxis]{};        // CAUTION, signed int - could go negative
    float nextPositionInMm[(uint8_t)axis::nmbrAxis]{};                //
};
