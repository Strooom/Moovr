// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#define F_BUS 60000000
#endif
#include <math.h>

//#include "gcode.h"				// apparently not needed
#include "event.h"                    // needed for definition of EventBuffer : Motion can signal events back to the mainController
#include "general.h"                  // needed for definition of eg. axis
#include "machineproperties.h"        // needed for definition of machineProperties : this is passed to Motion so it can include machine limitations eg. during speed calculations
#include "motionbuffer.h"             // needed for definition of motionItem, a component of Motion
#include "step.h"                     // needed for definition of stepperitem : this is an output calculated by Motion

class MotionCtrl {
  public:
    MotionCtrl(EventBuffer &);        // constructor, initializing the reference to the EventBuffer
    void run();
    void optimize();        // optimizes the motionBuffer speed profiles
    void append(gCodeParserResult &theParseResult);
    step getNextStep();

    void fill(step anItem);        // add an item to the buffer
    void output();
    void toString();

  private:
    MotionBuffer theMotionBuffer;                   // instance of the object
    EventBuffer &theEventBuffer;                    // reference to the eventBuffer of the parent mainController, so this class can push events there
    MachineProperties &theMachineProperties;        // reference to all the pysical properties of the machine - to be read frm .cfg file
    Overrides &theOverrides;                        // reference to override settings for feedrate and spindle-rpm

    // StateMachine
    MotionState theMotionState = MotionState::ready;
    MotionStrategy theStrategy = MotionStrategy::maximizeSpeed;        // should the machine maximize or minimze speed ?

    uint32_t timeInMotionTicks = 0;                                             // time elapsed in this motion, in discrete, integer timer ticks
    int32_t currentPositionInSteps[(uint8_t)axis::nmbrAxis] = {0, 0, 0};        // CAUTION, signed int!, could go negative during homing etc..
    float nextPositionInMm[(uint8_t)axis::nmbrAxis];                            // ???

    uint8_t junctionIndex = 0;        // index of which pair we ar ecurrently optimizing

    //// #### buffer ####
    //static constexpr float preScaler = 750;							// dividing the bus frequency (going into PIT0) by 750,  of PIT1, after prescaling through PIT0 chaining
    //static constexpr float timerFrequency = F_BUS / preScaler;		// results in PIT1 timer frequency of  80KHz
    //static constexpr float timerTick = 1.0F / timerFrequency;		// period of the PIT1 timer, this will automatically become the DIR setup time, as well as the STEP pulse width
    //static constexpr float hysteresis = 0.05f;						// hysteresis, to avoid setting steps forward and backwards due to floating point rounding errors. In fact the value put here is half of the hysteresis

    //static constexpr uint32_t bufferLength = 512;				// currently estimated as 25ms @ 10k steps/s and 2 entries per step
    //stepperItem buffer[bufferLength];						// the buffer itself

    //uint32_t stepBufferLevel = 0;									// number of items in the buffer
    //uint32_t stepBufferReadIndex = 0;								// index of next item to read
    //uint32_t stepBufferTotalTime = 0;								// total amount of timer cycles in buffer.. we try to keep this at ~ 10 ms
    //static constexpr uint32_t minBufferItems = 4;				// how much items to always have in the buffer
    //static constexpr uint32_t minStepBufferTotalTime = (timerFrequency / 100);		// how much time to keep in the buffer, in PIT1 ticks, is 1/100 or 10 ms of time
    //static constexpr uint32_t defaultReload = (timerFrequency / 400);				// 80K/400 = 200

    //uint32_t dirSetup = 0;											// bitPattern for setting Dir (Direction) before rising Step
    //uint32_t stepRise = 0;											// bitPattern for rising edge of Step signal
    //uint32_t stepFall = 0;											// bitPattern for falling edge of Step signal

    //uint32_t lastOut = 0;											// last bits written to IOs, in Motion::output()- needed to detect any change, so we only write when something changes
    //uint32_t lastReload = 0;										// timeInMotionTicks at time of last write to outputs - needed to derive reload value for next output.
};
