Next Steps :
* solve gCodeParser and motionController reset, needed for a fresh start when homing




Todo : think about how to handle errors
* not reaching the closing or opening of a switch (in time / in space)
* receiving events on other switches than the one monitoring
 

Todo : homing uses gcode to generate motions, however, it should not change the modal state of the machine. So we must save the modal state and afterwards restore it..

ToDo : we need to capture the position on the opening of the switch as the ZERO


