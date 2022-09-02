
Nxt Tasks
=========

* make homing work
* think about how to make machineProperties more structured..
  - motors (j, a, d, v)
  - axis (s, limits)
  




TODO :

1. Check public, protected, private for all properties and methods
2. Check if parameters can be made const
3. Get rid of char and replace with uint8_t uint32_t
4. Add critical sections for access to rx/txBuffer !! sei(); cli();
5. Add a lastError and test that it triggers when it should and not if it shouldn't
6. make parameters which are needed to initialize motionCtrl and stepBuffer, defaults, which can be passed in constructor
    -> if you don't pass anything, it will take values from machineProperties
    -> if you want, for testing, you can pass something else



# How to get collaboration from the community
* provide some standard builds which can be flashed on a standard HW setup
* provide a YouTube channel with video's on how the software works
* make it work with standard GRBL gcode senders..



Machine properties could be organized in two ways :
* property[axis] eg. vMax[axis::X]
* axis object, with properties per instance.. eg axis X {vMax{100}}

Question is what is the easiest way to iterate over axis or to get properties for a certain axis.
Currently we are often passing axis as an index io object.. this would not work in the second approach

