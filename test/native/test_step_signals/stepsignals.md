Introduction
============

Stepper motors are typically driven by a power-electronics driver. This driver receives a STEP and DIR signal and translates it to driving voltages on the motor coils. 
Even more advanced servo (closed loop) motors also use the STEP / DIR interface.

The STEP signal expects a pulse for each step the motor should take
The DIR signal indicates the direction the motor should turn.

Each driver will have the following signal requirements :
* a minimum STEP pulse duration
* a minimum DIR setup time, ie. the DIR signal must remain stable some time before the STEP pulse and during the STEP pulse

In order to drive a collection of stepper motors, all the STEP and DIR signals are combined into a single 32-bit word (stepSignals), resulting in a maximum of 16 motors.
Outputting this value, drives all motors simultaneously.

Making a motor take a step requires the following:
1* set DIR to the correct value
2* wait the required setup time
3* set STEP high
4* wait the required minimum pulse duration time
5* set STEP low again



TestPlan
========

* check binary signals when taking a step, forward, backward, one axis, multiple axis
* check shifting of values
* check output
    - value
    - ticks

* isTimedOut : think about boundary condition
* 