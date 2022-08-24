
Nxt Tasks
=========

* build a minimal main application, binding together all subsystems


Application Initialization
==========================
* instance of eventBuffer
* instance of motionControl
* instance of gCodeParser
* instance of simplifiedMotion to pass results from gCodeParser to motionControl
* instance of stepBuffer

* theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput( ... );
* theMotionCtrl.theSampleTime.setminStepPulseWidth( ... );

* load some gCode
* keep stepBuffer filled from motionControl



TODO :

1. Check public, protected, private for all properties and methods
2. Check if parameters can be made const
3. Get rid of char and replace with uint8_t uint32_t
4. Add critical sections for access to rx/txBuffer !! sei(); cli();
5. Add a lastError and test that it triggers when it should and not if it shouldn't










