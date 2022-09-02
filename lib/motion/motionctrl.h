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
    void initialize(uint32_t maxTicksSinceLastOutput, float minStepPulseWidth);
    void append(simplifiedMotion& aSimpleMotion);
    void start();
    void hold();
    void resume();
    void stop();
    void flushMotionBuffer();
    void resetMachinePosition();
    void optimize();

    motionState getState() const;
    bool isRunning() const;
    motionStrategy theStrategy() const;
    
    step calcNextStepperMotorSignals();

    int32_t machinePositionInSteps[nmbrAxis]{0}; // TODO : this needs to be private, but made it public to make homing easier fttb

#ifndef unitTesting
  private:
#endif
    motionState theMotionCtrlState = motionState::stopped;        // eg running, stopping or stopped
    overrides theOverrides;                                       // override values for feedrate and spindlespeed
    sampleTime theSampleTime;                                     // object keeping track of sampling the trajectory at regular time intervals
    motionBuffer theMotionBuffer;                                 // buffer with motion segments to be executed
    stepSignals theStepSignals;                                   // signals to be sent to the motors with their timing

    float vJunction(uint32_t left, uint32_t right) const;              // max speed at the boundary of two motion segments
    bool needStepForward(uint32_t anAxis, float positionInMm);         //
    bool needStepBackward(uint32_t anAxis, float positionInMm);        //
    void stepForward(uint32_t anAxis);                                 //
    void stepBackward(uint32_t anAxis);                                //
    void optimizePair(int32_t junctionIndex);                          //
    void move();                                                       // decide for each axis if it should make a step when going from currentPosition to newPosition
    bool isOptimal{false};
};
