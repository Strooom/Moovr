#include "logging.h"


Logger::Logger()
    {
    bufferLevel = 0;
    logBuffer[0] = 0x00;			// initialize the logBuffer to empty string
    outputIsAvailable = false;
    strcpy(logBuffer, "logBuffer initialized\n");
    }

void Logger::log()
    {
    strcat(logBuffer, logLine);
    }

void Logger::logNow()
    {
    strcat(logBuffer, logLine);
    output();
    }

void Logger::log(const char achar)
    {
    // check buffer overflow..
	char tmp[2];
	tmp[0] = achar;
	tmp[1] = 0;
	strcat(logBuffer, tmp);
    };

void Logger::logNow(const char achar)
    {
    log(achar);
    output();
    };


void Logger::log(const char *text)
    {
    // check buffer overflow..
    strcat(logBuffer, text);
    };

void Logger::logNow(const char * text)
    {
    log(text);
    output();
    };


void Logger::log(uint32_t number)
    {
    char tmp[12];
    itoa(number, tmp, 11);
    strcat(logBuffer, tmp);
    }


void Logger::logNow(uint32_t number)
    {
    log(number);
    output();
    }

void Logger::log(const char* text, uint32_t number)
    {
    strcat(logBuffer, text);
    char tmp[12];
    itoa(number, tmp, 11);
    strcat(logBuffer, tmp);
    }

void  Logger::logNow(const char* text, uint32_t number)
    {
    log(text, number);
    output();
    }

void  Logger::log(float number)
    {}
void  Logger::logNow(float number)
    {}

void Logger::output()
    {
#ifdef WIN32
    printf(logBuffer);
    logBuffer[0] = 0x00;			// reset logBuffer to empty string
#else
    if (outputIsAvailable)
        {
        Serial.print(logBuffer);
        logBuffer[0] = 0x00;			// reset logBuffer to empty string
        }
#endif
    }