# How to measure / monitor the load on the MCU

* many parameters influence the load on the MCU
  - what gCode is being parsed
  - number of axis moving simultaneously
  - type of motion, eg linear vs helical

At the top-level application, there is a main loop, which checks/polls all subsystems if some tasks are needed.
When nothing needs to be done, this main loop executes in the minimum time, or a maximum number of times per second

The stepBuffer contains a number of steps which need to be sent to motors with correct timing. 
This buffer underflowing, results in a major malfunction of the motion controller.
The main loop checks if this stepBuffer needs to be filled.
To accomodate for MCU being busy on other stuff, the stepBuffer is always filled with a number of steps representing a certain amount of time of operation : minStepBufferTotalTime
So when the CPU is busy for less time than minStepBufferTotalTime, it will refill this buffer in time before it underflows.

When the CPU is busy for the minStepBufferTotalTime, this represents a load of 100%
When this occurs, the time taken for 1 iteration of the main loop equals minStepBufferTotalTime

So a good estimate for MCU load is :

100 / (nmbr of iterations in main loop per minStepBufferTotalTime) 