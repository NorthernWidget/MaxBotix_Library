/******************************************************************************
Maxbotics.h
Library for interfacing with Maxbotics rangefinders for use with enviromental sensing
Bobby Schulz @ Northern Widget LLC
Andy Wickert @ Northern Widget LLC
7/16/2018
https://github.com/NorthernWidget-Skunkworks/Maxbotics_Library

This script is used to interface to the Maxbotics sensors and return distance to target, along with other
statistics

Boss: "I just heard that light travels faster than sound. I'm wondering if I should shout when I speak, just so my lips appear to sync-up with my words.""
Dilbert (thought): "A little knowledge can be a ridiculous thing."
-Scott Adams

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef MAXBOTIX_h
#define MAXBOTIX_h

#include "Arduino.h"

/////////////
// GLOBALS //
/////////////

uint8_t RxPin;
uint8_t npings;
bool writeAll;
uint8_t ExPin;
bool RS232;
uint16_t minRange_mm;
uint16_t maxRange_mm;

class Maxbotix
{
	public:
		Maxbotix();
		uint8_t begin(uint8_t _RxPin, uint8_t _npings=1, bool _writeAll=false, \
                  uint8_t _ExPin=-1, bool _RS232=false, \
                  uint16_t _minRange_mm=500, uint16_t _maxRange_mm=5000);
		float GetRange();
		float GetRanges();
		String GetHeader();
		String GetString();

	private:
		
};

#endif
