# Different types of coordinates in play in the Moovr application

## Machine Coordinates
This is the position of the physical toolhead of the machine. 
Origin is:
 * the position of the toolhead when the controller booted
 * the position at an offset of all limitswitches of all axis, after homing

 These coordinates are internally kept in int32_t machinePositionInSteps[nmbrAxis]
 They can be converted to mm through <xxx> TODO


 ## GCODE Coordinates = World Coordinates
 These are the positions calculated from gcode.
 They are internally kept as double-precision floating point