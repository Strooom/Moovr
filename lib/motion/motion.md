# simplifiedMotion
A simplifiedMotion is a motion that does not take into account physical limitations of the machine such as max acceleration and maximal speed.
As such it contains a trajectory, feedrate or duration, and some peripherals on/off states

# motion
A (true) motion does take physical limitations into account. So it has extra properties like acceleration, etc.

A motion is derived from a simplifiedMotion by calculating the additional properties from the simplifiedMotion and the machine limitations

A motion is composed of several subcomponents
* trajectory
* speedprofile
* peripherals

A speedprofile controls at what speed the machine follows the trajectory.
It has 3 phases :
* acceleration
* cruise
* deceleration

When the user overrides the feedrate, or issues a feedhold, the speedprofile will be recalculated.
This recalculation may occur in the middle of a motion which is being executed.
The part of the motion which is already executed, can of course no longer be changes.
So the speedprofile has an additional part 'passed' which represents the part already done and no longer possible to change
So now a speedprofile has 4 phases :
* passed
* acceleration
* cruise
* deceleration

In some cases the motion will have a slower cruise speed than the previous and next motion. In that case, we have
* passed
* deceleration
* cruise
* acceleration

So the more generic case is :
* passed
* acceleration / deceleration
* cruise
* acceleration / deceleration


# speedProfile Order
the acceleration / deceleration phase can happen in two ways :
1. firstOrder : with a fixed acceleration - infinite jerk
2. secondOrder : with a limited jerk

the secondOrder profile requires more computation, but results in smoother machine movement.
