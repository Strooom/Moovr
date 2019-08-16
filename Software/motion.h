// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef motion_h
#define motion_h

#ifndef WIN32
#include <Arduino.h>
#else
#include <inttypes.h>
#endif

#include "event.h"					// needed for definition of EventBuffer
#include "gcode.h"					// needed for definition of eg. axis

enum class MotionState : uint8_t {ready, running, stopping, stopped};							// State of the MotionController
enum class MotionStrategy : uint8_t { minimizeSpeed, maximizeSpeed, nmbrStrategies };			// Strategy for optimizing the motion
enum class MotionSpeedProfile : uint8_t { zeroOrder, firstOrder, secondOrder, nmbrProfiles };	// Shape of the speedprofile : zero, first or second order

struct stepperItem
    {
    uint32_t out;												// 16-bit pattern to write to port IO's.. as there is nothing connected to the upper 16 bits of PORTC, there is no need to write 32bits
    uint32_t reload;											// 32-bit value to write in Timer Reload register. TODO : here I could also use 16 bits io 32 as the values will never need more than 16 bits... however the register is 32 bits..
    };

class MachineProperties
    {
    public:
        struct Motors
            {
            float jMax = 100.0f;														// [mm/s^3]
            float aMax[(uint8_t)axis::nmbrAxis] = { 100.0f, 100.0f, 100.0f };			// [mm/s^2]
            float dMax[(uint8_t)axis::nmbrAxis] = { -100.0f, -100.0f, -100.0f };		// [mm/s^2]
            float vMax[(uint8_t)axis::nmbrAxis] = { 200.0f, 200.0f, 200.0f };			// [mm/s]
            float sMax[(uint8_t)axis::nmbrAxis] = { 600.0f, 800.0f, 130.0f };			// [mm]
            float stepsPerMm[(uint8_t)axis::nmbrAxis] = { 50.0f, 50.0f, 50.0f };		// [1/mm]
            } motors;

        struct Limits
            {
            bool hasLimitsMin[(uint8_t)axis::nmbrAxis];		// limit switches towards the negative direction of the Axis
            bool hasLimitsMax[(uint8_t)axis::nmbrAxis];		// limit switches towards the positive direction of the Axis
            float maxLimitswitchTravel = 2.0f;				// [mm]
            } limits;

        struct Spindle
            {
            bool hasVariableSpindle;
            bool hasSpindledirection;
            float minSpindleRPM;
            float maxSpindleRPM;
            } spindle;

        struct Coolant
            {
            bool hasCoolantFlood;
            bool hasCoolantMist;
            } coolant;

        double minLengthSProfile{ 1000.0f };					// [mm] all motions with a length smaller will be 2nd order T-profile - larger will be 3rd order S-profile
    };

struct Overrides
    {
    float feedOverride = 1.0f;							// feed override factor [0.1 .. 10]
    float spindleOverride = 1.0f;						// spindle speed overrride factor [0.1 .. 10]
    };

class MotionPart
    {
    public:
        MotionSpeedProfile theProfile;						// 0, 1st (T) or 2nd (S) order speedProfile

        // Inputs
        float vStart;										// speed at entry of the section [mm/s]
        float vEnd;											// speed at exit of the section [mm/s]
        float aMax;											// maximum acceleration along the path [mm/ s^2]
        float dMax;											// maximum deceleration along the path [mm/ s^2]
        float jMax;											// maximum jerk = machine constant [mm / s^3]

        // Outputs - Distance
        float s1;											// length of phase 1
        float s2;											// length of phase 2
        float s3;											// length of phase 3
        float length;										// total length of the motionPart [mm]

        // Outputs - Timing
        float d1d3;											// duration of phase 1 = duration of phase 3. Zero for T-profile [s]
        float d2;											// duration of phase 2. Is the only phase for T-profile [s]

        float calcLength() const;							// calculate length of a section given vStart, vEnd, aMax, dMax, jMax
        float calcSpeed() const;							// calculate the exit speed to a T-profile section, given vStart, (wanted but not reached) vEnd, length, aMax, dMax
        void calculate();									// input vStart, vEnd, length,....   then calculate timings..

        MotionPart(MotionSpeedProfile theProfile, float vStart, float vEnd, float aMax, float dMax, float jMax);
        MotionPart() {}; // Todo default constructor
    };

class MotionItem
    {
    public:
        // ##########################
        // ### Space - Trajectory ###
        // ##########################
        MotionType theMotionType;
        float startPosition[(uint8_t) axis::nmbrAxis];	// mm
        float deltaRealTime[(uint8_t) axis::nmbrAxis];	// mm
        axis arcAxis0;
        axis arcAxis1;
        float arcCenter0;								// mm
        float arcCenter1;								// mm
        float startAngle;								// rad
        float radius;									// mm

        float s1;										// distance travelled to end of phase 1
        float s2;										// distance travelled to end of phase 2
        float s3;										// distance travelled to end of phase 3 = end of acceleration = start of cruise
        float s4;										// distance travelled to end of phase 4 = end of cruise = start of deceleration
        float s5;										// distance travelled to end of phase 5
        float s6;										// distance travelled to end of phase 6
        float sMotion;									// total length of the Motion [mm]

        // #########################
        // ### Time - Speed      ###
        // #########################
        MotionSpeedProfile theMotionProfile;			// Shape of the speedprofile : 0, 1st or 2nd order
        float vFeed;									// this is the (max) speed as 'wanted' by the GCode
        float vMax;										// this is the max speed as limited by machine constraints
        float vStart;									// speed at entry of the motion [mm/s]
        float vEnd;										// speed at exit of the motion [mm/s]
        float vMid;										// speed in the cruise phase of the motion
        float v1v3;
        float v5v7;
        float aMax;										// this is the max acceleration as limited by machine constraints [mm/s^2], >=0
        float dMax;										// this is the max decelerationspeed as limited by machine constraints [mm/s^2], <=0
        float jMax;										// this is the max jerk as limited by machine constraints [mm/s^3], >0

        float t1;										// time where phase 1 ends [s]
        float t2;										// time where phase 2 ends [s]
        float t3;										// time, in this Motion, the acceleration phase ends [s]
        float t4;										// time, in this Motion, the deceleration phase starts [s]
        float t5;										// time where phase 5 ends [s]
        float t6;										// time where phase 6 ends [s]
        float tMotion;									// total time this Motion lasts [s]

        float tStop;									// time at which the motion will come to a stop - for feedhold.. if no stop needed, then this points (1.0 s) beyond tMotion, so the time > tStop will never assert


        // ##############################
        // ### Spindle & Coolant      ###
        // ##############################

        rotationDirection spindledirection;
        float spindleSpeed;
        bool coolantFlood;
        bool coolantMist;

    public:

        void calcSpeedLimits(gCodeParserResult &theParseResult, MachineProperties &theMachineProperties);		// calculates vMax, aMax, dMax so all Axis and arcs are within machine boundaries
        float calcTriangular(MotionStrategy theStrategy);
        float calcOtherV(MotionStrategy theStrategy, bool forward);

        float s(float t);									// calculates s : distance travelled along the trajectory on time t
        float v(float t);									// calculates v : speed along the trajectory on time t
        float a(float t);									// calculates a : acceleration (or deceleration, which is just negative) along the trajectory on time t
        //float vMid2(Overrides &theOverrides);				// calculates maximum vMid from vFeed, vLimit and feed overrides
        void optimize(MotionStrategy theStrategy, Overrides &theOverrides, MachineProperties &theMachineProperties);			// optimizes this motionItem
        void fromParts(MotionPart &left, MotionPart &right, float d4, float len4);	// load the MotionItem properties, from two MotionParts
        void adjustRemaining();

        void toString();									// print the properties of this item, for testing

    };

class Motion
    {
    private:
		EventBuffer &theEventBuffer;											// reference to the eventBuffer of the parent mainController, so this class can push events there

        // StateMachine
        MotionState theMotionState = MotionState::ready;
        MotionStrategy theStrategy = MotionStrategy::maximizeSpeed;				// should the machine maximize or minimze speed ?

        // MachineProperties
        MachineProperties  theMachineProperties;								// all the pysical properties of the machine - to be read frm .cfg file
        Overrides theOverrides;													// override settings for feedrate and spindle-rpm

        // #### motionBuffer ####
        static constexpr uint8_t motionBufferLength = 32;						// max 256, as addressed by uin8t_t. Length should be a compromise : large enough to allow good speed-optimization. Not too large to consume ram and cpu in optimizing
        MotionItem motionBuffer[motionBufferLength];							// The buffer holding all motions to be executed
        uint8_t motionBufferReadIndex = 0;										// First-to-be-executed or currently-being-executed motion
        uint8_t motionBufferLevel = 0;											// number of motions in the buffer

        uint32_t timeInMotionTicks = 0;											// time elapsed in this motion, in discrete, integer timer ticks
        int32_t currentPositionInSteps[(uint8_t)axis::nmbrAxis] = { 0,0,0 };	// CAUTION, signed int!, could go negative during homing etc..
        float nextPositionInMm[(uint8_t)axis::nmbrAxis];						// ???

        uint8_t junctionIndex = 0;												// index of which pair we ar ecurrently optimizing

#ifdef WIN32
#define F_BUS 60000000
#endif

        // stepBuffer parameter estimations

        // Example 1: max stepping frequency = 10k steps/s
        // Oversampling x8 -> 80k samples/s
        // 60M (PIT timer clock) / 80K = 750
        // idle stepBuffer should contain 4 actions, for a total time of ~10ms -> each item 2.5ms
        // 2.5ms * 80K = 200 default reload value

        // Example 2: max stepping frequency = 50k steps/s
        // Oversampling x8 -> 400k samples/s
        // 60M / 400K = 150
        // reload = 1000

        // #### stepBuffer ####
        static constexpr float preScaler = 750;							// dividing the bus frequency (going into PIT0) by 750,  of PIT1, after prescaling through PIT0 chaining
        static constexpr float timerFrequency = F_BUS / preScaler;		// results in PIT1 timer frequency of  80KHz
        static constexpr float timerTick = 1.0f / timerFrequency;		// period of the PIT1 timer, this will automatically become the DIR setup time, as well as the STEP pulse width
        static constexpr float hysteresis = 0.05f;						// hysteresis, to avoid setting steps forward and backwards due to floating point rounding errors. In fact the value put here is half of the hysteresis

        static constexpr uint32_t stepBufferLength = 512;				// currently estimated as 25ms @ 10k steps/s and 2 entries per step
        stepperItem stepBuffer[stepBufferLength];						// the buffer itself

        uint32_t stepBufferLevel = 0;									// number of items in the buffer
        uint32_t stepBufferReadIndex = 0;								// index of next item to read
        uint32_t stepBufferTotalTime = 0;								// total amount of timer cycles in buffer.. we try to keep this at ~ 10 ms
        static constexpr uint32_t minStepBufferItems = 4;				// how much items to always have in the buffer
        static constexpr uint32_t minStepBufferTotalTime = (timerFrequency / 100);		// how much time to keep in the buffer, in PIT1 ticks, is 1/100 or 10 ms of time
        static constexpr uint32_t defaultReload = (timerFrequency / 400);				// 80K/400 = 200

        uint32_t dirSetup = 0;											// bitPattern for setting Dir (Direction) before rising Step
        uint32_t stepRise = 0;											// bitPattern for rising edge of Step signal
        uint32_t stepFall = 0;											// bitPattern for falling edge of Step signal

        uint32_t lastOut = 0;											// last bits written to IOs, in Motion::output()- needed to detect any change, so we only write when something changes
        uint32_t lastReload = 0;										// timeInMotionTicks at time of last write to outputs - needed to derive reload value for next output.


    public:
		Motion(EventBuffer &);											// constructor, initializing the reference to the EventBuffer
        void run();
        void optimize();												// optimizes the motionBuffer speed profiles
        void append(gCodeParserResult &theParseResult);
        bool isReady(void) const;										// is the Motion planner ready to take new items ?
        stepperItem getNextStep();

        void fill(stepperItem anItem);									// add an item to the buffer
        void output();
        void toString();
    };

#endif