// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "event.h"

void EventBuffer::pushEvent(Event theEvent)
    {
#ifndef WIN32
    cli();		// no Interrupts : this function can be called from main thread and fron interrupt handlers
#endif
    if (eventBufferLevel < eventBufferLength)
        {
        eventBuffer[(eventBufferReadIndex + eventBufferLevel) % eventBufferLength] = theEvent;		// write new event at next writeIndex = readIndex + Level
        eventBufferLevel++;																			// adjust level to one item more
        }
    else
        {
        // Event buffer Overflow...
        }
#ifndef WIN32
    sei(); // re-enable interrupts
#endif
    }

Event EventBuffer::popEvent()
    {
    Event theEvent = Event::none;
#ifndef WIN32
    cli();		// no Interrupts : this function can be called from main thread and fron interrupt handlers
#endif
    if (eventBufferLevel > 0)
        {
        theEvent = eventBuffer[eventBufferReadIndex];							// read the oldest event
        eventBufferReadIndex = (eventBufferReadIndex + 1) % eventBufferLength;	// advance readIndex to next position
        eventBufferLevel--;														// adjust level to one item less
        }
    else
        {
        // Event buffer Underflow...
        }
#ifndef WIN32
    sei(); // re-enable interrupts
#endif
    return theEvent;
    }

bool EventBuffer::hasEvents()
    {
#ifndef WIN32
    cli();		// no Interrupts : this function can be called from main thread and fron interrupt handlers
#endif
    bool hasEvents = (eventBufferLevel > 0);
#ifndef WIN32
    sei(); // re-enable interrupts
#endif
    return hasEvents;
    }
