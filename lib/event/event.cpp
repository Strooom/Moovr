#include "event.h"

const char* toString(event anEvent) {
    switch (anEvent) {
        case event::none:
            return "none";
            break;
        case event::emergencyStopButtonPressed:
            return "emergencyStop button pressed";
            break;
        case event::emergencyStopButtonReleased:
            return "emergencyStop button released";
            break;
        case event::feedHoldResumeButtonPressed:
            return "feed Hold/Resume button pressed";
            break;
        case event::feedHoldResumeButtonReleased:
            return "feed Hold/Resume button released";
            break;
        case event::probeSwitchClosed:
            return "probeSwitch closed";
            break;
        case event::probeSwitchOpened:
            return "probeSwitch opened";
            break;
        case event::limitSwitchXMinClosed:
            return "limitSwitch XMin closed";
            break;
        case event::limitSwitchYMinClosed:
            return "limitSwitch YMin closed";
            break;
        case event::limitSwitchZMinClosed:
            return "limitSwitch ZMin closed";
            break;
        case event::limitSwitchXMaxClosed:
            return "limitSwitch XMax closed";
            break;
        case event::limitSwitchYMaxClosed:
            return "limitSwitch YMax closed";
            break;
        case event::limitSwitchZMaxClosed:
            return "limitSwitch ZMax closed";
            break;
        case event::limitSwitchXMinOpened:
            return "limitSwitch XMin opened";
            break;
        case event::limitSwitchYMinOpened:
            return "limitSwitch YMin opened";
            break;
        case event::limitSwitchZMinOpened:
            return "limitSwitch ZMin opened";
            break;
        case event::limitSwitchXMaxOpened:
            return "limitSwitch XMax opened";
            break;
        case event::limitSwitchYMaxOpened:
            return "limitSwitch YMax opened";
            break;
        case event::limitSwitchZMaxOpened:
            return "limitSwitch ZMax opened";
            break;
        case event::motionAdded:
            return "motion added";
            break;
        case event::motionCompleted:
            return "motion completed";
            break;
        case event::allMotionsCompleted:
            return "all motions completed";
            break;
        case event::motionStarted:
            return "motion started";
            break;
        case event::motionStopped:
            return "motion stopped";
            break;
        case event::motionBufferOverflow:
            return "motionBufferOverflow";
            break;
        case event::homingStarted:
            return "homingStarted";
            break;
        case event::homingCompleted:
            return "homingCompleted";
            break;
        case event::probingStarted:
            return "probingStarted";
            break;
        case event::probingCompleted:
            return "probingCompleted";
            break;
        case event::stepBufferUnderflow:
            return "stepBufferUnderflow";
            break;
        case event::stepBufferOverflow:
            return "stepBufferOverflow";
            break;
        default:
            return "unknown";
            break;
    }
}

