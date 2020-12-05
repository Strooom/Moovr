#pragma once

#ifndef  WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "gcodeparseresult.h"
#include "machineproperties.h"
#include "motionitemtrajectory.h"
#include "motionitemspeedprofile.h"
#include "motionitemperipheral.h"

class MotionItem
    {
	public:
		void set(const gCodeParserResult &theParseResult, const machineProperties &theMachineProperties, const MotionStrategy &theStrategy, const overrides &theOverrides);			// sets the properties of an Item based upon inputs from gcodeParseResult and machineProperties

		void limit(const machineProperties &theMachineProperties);													// calculate vMax, aMax and dMax from trajectory and machine limits
		void optimize(MotionStrategy theStrategy, const overrides &theOverrides);									// optimizes this motionItem

		void adjustRemaining();
		float calcTriangular(const MotionStrategy theStrategy);
		float calcOtherV(const MotionStrategy theStrategy, bool forward);

		float s(float t) const;								// calculates s : distance travelled along the trajectory on time t
		float v(float t) const;								// calculates v : speed along the trajectory on time t
		float a(float t) const;								// calculates a : acceleration (or deceleration, which is just negative) along the trajectory on time t

		void print() const;									// prints a diagram to Serial for evaluating and debugging
		void plot(char type, float xMin, float xMax, uint32_t xSteps, float yMin, float yMax, uint32_t ySteps) const;		// plots a chart to Serial for evaluating and debugging. Type is 'a', 'v' or 's'

		//float vMid2(overrides &theOverrides);				// calculates maximum vMid from vFeed, vLimit and feed overrides
		//void fromParts(const SpeedProfileAccDec &left, const SpeedProfileAccDec &right, float d4, float len4);	// load the MotionItem properties, from two MotionParts

    //private:
		MotionType theType;
		MotionItemTrajectory theTrajectory;					// contains all properties for defining the trajectory in space
		MotionItemSpeedProfile theSpeedProfile;				// contains all properties for defining the speed over the trajectory in time
		MotionItemPeripheral thePeripherals;				// contains all settings for peripherals over this part of the trajectory
    };