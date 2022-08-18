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
    void append(simplifiedMotion& aSimpleMotion);
    void start();
    void hold();
    void resume();
    void stop();

    void optimize();

    motionState getState() const;
    bool isRunning() const;
    motionStrategy theStrategy() const;

    step calcNextStepperMotorSignals();

#ifndef unitTesting
  private:
#endif
    motionState theMotionCtrlState = motionState::stopped;        //
    overrides theOverrides;
    sampleTime theSampleTime = sampleTime(1);        // TODO : set a correct sample interval
    motionBuffer theMotionBuffer;                    //
    stepSignals theStepSignals;                      //

    point currentPosition;
    point nextPosition;

    float vJunction(uint32_t left, uint32_t right) const;        //
    // void positionInSteps();                                                                           //
    // void positionInMm(uint32_t anAxis, float sNow, motionTrajectory& currentMotionTrajectory);        //
    bool needStepForward(uint32_t anAxis);           //
    bool needStepBackward(uint32_t anAxis);          //
    void optimizePair(int32_t junctionIndex);        //
    void move();                                     // from currentPosition to newPosition
    bool isOptimal{false};
};
