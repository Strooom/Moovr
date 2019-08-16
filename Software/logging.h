// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################


/*	Purpose of this object is to declare a global object which helps in logging events
	the code. When it is declared at the top of the .ino file, its constructor should
	be called as the very first thing.
	From then on, all subsequent constructors can use the object and start logging
	events. Initially, the events being logged will go into the buffer. Then later-on,
	when the output-channel becomes available (eg usb-serial or file on SD-card) the
	buffer will be flushed and further logevents will be directed to the output.
*/

#ifndef logging_h
#define logging_h

#ifndef WIN32
#include <Arduino.h>
#else
#include <inttypes.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

#endif


#define log_all						// switches all logging output via Serial On or Off

#ifdef log_all


class Logger
    {

    private:
        static constexpr uint32_t logBufferSize = 4096;
        uint32_t bufferLevel;

	protected:
		char logBuffer[logBufferSize];		// buffer to store logdata when output is not yet available

	public:
		char logLine[128];					// temp storage for 1 line of text to be the output for snprintf()
        Logger();
        bool outputIsAvailable;
        bool logTimestamps;
		void log();
		void logNow();
		void log(const char achar);
		void logNow(const char achar);
		void log(const char* text);
        void logNow(const char * text);
        void log(uint32_t number);
        void logNow(uint32_t number);
        void log(float number);
        void logNow(float number);
        void log(const char* text, uint32_t number);
        void logNow(const char* text, uint32_t number);
        void output();
    };






#define log_mainController_class				// logging output for the mainController class/object


#define log_gCode_class						// logging output for the gCode parser


#define log_hostInterface				// logging for the hostInterface


#define log_motion_class


#define log_stepper_class


#endif // log_all






#endif


