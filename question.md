I need some advice on how to organize my project in platformio.ini 
The project is a motion controller firmware, and is intended to be used in multiple applications eg:
    * a 3-axis CNC
    * a 2-axis Plotter

The question is how to organise the code in the project, in the correct and most efficient way.

1. Generic Code : a large part of the code is generic, ie. it is common to all possible applications : the gCode parser, motion calculations, etc. 
This code is being unit tested in a 'native' environment on the development machine as well as on the target hardware.

2. A small part of the code will be application specific :
    * depending on the type of machine : number of axis, limit switches, etc
    * depending on the specific MCU / motherboard that it runs on.

    The application specific code consists of:
    * a dedicated main.cpp (creating instances of main components and making them work together)
    * some application / hardware specific 'library' code : eg :
        - some application specific include files (eg. axis.h) 
        - some target hardware specific files, implementation of UART, HW-timers, GPIO, etc.

3. Finally the project uses some external libraries, eg. ArduinoJson


So for this example I set up 3 [env] environments in platformio.ini :
    [env:native]
    [env:cnc3axis]
    [env:plotter2axis]

All of the generic code goes under \lib where it can be found for both the unit tests and the application builds.

External libraries are found using a lib_deps entry, eg.
    [env]
    lib_deps = bblanchon/ArduinoJson @ ^6.19.4

Each of the [env:xxx] sections can select it's own main application using build_src_filter = +<cnc3axis/*>

The remaining question is where to put the application-specific 'library' code. (note that these are not really full-blown, self-contained libraries, but files containing helper classes).
This I tried to arrange with a lib_extra_dirs clause eg. lib_extra_dirs = lib_target/cnc3axis where are application specific code goes in a subfolder lib_target/cnc3axis/general

