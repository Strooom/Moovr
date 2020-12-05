// #############################################################################
// ###                                                                       ###
// ### General Purpose Logging toolkit for MicroControllers                  ###
// ### https://github.com/Strooom/Logging                                    ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// #############################################################################

// When developing on microcontrollers, it's often not easy to know what's going on inside.
// A basic solution is to include 'print' statements in your code, which will output to some kind of monitor.
// But this monitoring output could differ from case to case : UART, SD-card, ...
// And further, this output may not be available until after some initialization.
// This logging solution provides a solution :
// * It creates a global object, so it is available from the real start of your program
// * It will buffer the logged items, until the output becomes available
// * It can easily be adapt to any kind of output
//
// V 1.0.0 25-08-2020 First release
// V 2.0.0 08-10-2020 Made it simpler by providing less public methods, but making them more flexible. Did extensive unit testing for robustness.
// Example of using snprintf :  theLog.snprintf(loggingLevel::Debug, "Error in %s on line %d\n", __FILE__, __LINE__);
// V2.1.0 11-11-2020 : Added the concept of a stack for loggingLevels (currently fixed to 4 elements depth). This allows you to temporarily change the loggingLevel and afterwards return to the previous level with simple push pop operation
// V2.2.0 05-12-2020 : Added output to std::cout when compiled for Win32 io Arduino target

#pragma once

#ifndef WIN32
#include <Arduino.h>
#else
#include <inttypes.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#endif
#include <stdarg.h>

enum class loggingLevel : uint8_t {
    None     = 0,        // Not logging anything at all
    Critical = 1,        // only logging critical errors
    Error    = 2,        // logging all errors
    Warning  = 3,        // logging all warnings and all errors
    Info     = 4,        // logging errors, warnings and useful info (release)
    Debug    = 5         // logging everything (including debug info)
};

class uLog {
  public:
    explicit uLog(const loggingLevel initialLoggingLevel = loggingLevel::Debug, const bool includeTimestamp = true);        // constructor

    // ------------------------------
    // configuring the logging object
    // ------------------------------
    void pushLoggingLevel(loggingLevel newLevel);           // set the loggingLevel, current level on the stack
    loggingLevel popLoggingLevel();                         // return to previous loggingLevel, popping one level from the stack, returns the new level
    loggingLevel getLoggingLevel() const;                   // returns the current loggingLevel
    void setOutputIsAvailable(bool isAvailable);            // set the outputIsAvailable
    void setIncludeTimestamp(bool includeTimestamp);        // set the includeTimestamp option

    // ------------------------------
    // logging services
    // ------------------------------
    void log(loggingLevel theLevel, const char* aText);                   // appends msg to loggingBuffer whithout trying to output immediately
    void output(loggingLevel theLevel, const char* aText);                // appends msg and tries to output immediately - this output may be blocking
    void snprintf(loggingLevel theLevel, const char* format, ...);        // does a printf() style of output to the logBuffer. It will truncate the output according to the space available in the logBuffer
    void flush();                                                         // outputs everything already in the buffer

    // ----------------------------------
    // internal data and helper functions
    // ----------------------------------

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    void output();                                                      // send to output, Serial for the time being
    void logTimestamp();                                                // add timestamp to the buffer
    bool checkLoggingLevel(loggingLevel itemLoggingLevel) const;        // check if this msg needs to be logged, comparing msg level vs logger level
    bool checkLogBufferLevel(uint32_t itemLength) const;                // check if there is sufficient space in the buffer to add the msg
    static constexpr uint32_t maxItemLength = 128;                      // Maximum length of new item to be logged. Will be an upper limit to all C-style string like strnlen()
    static constexpr uint32_t bufferLength  = 1024;                     // Length of the buffer to temporarily store the logging data, until being sent to an ouptut
    char logBuffer[bufferLength + 1];                                   // buffer to store logdata when output is not yet available. + 1 for terminating zero
    uint32_t bufferLevel = 0;                                           // keeping track of how much data is in the buffer
    loggingLevel theLoggingLevel[4];                                    // controls what amount of information to log : from nothing to everything. There is a stack of 4 levels, so pushing and popping is possible
    bool outputIsAvailable                    = false;                  // by default the output is not available and needs to be activated first
    bool includeTimestamp                     = false;                  // by default the output lines are not prefixed with a timestamp
    static constexpr uint32_t timestampLength = 6;                      // number of digits to use for timestamps
};
