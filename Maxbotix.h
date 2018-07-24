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
#include <math.h>
#include <SoftwareSerial.h>

/////////////
// GLOBALS //
/////////////



// Define ranges here instead of in function, w/ pointer?

class Maxbotix
{
	public:
		Maxbotix();
		bool begin(uint8_t _RxPin, uint8_t _nPings=1, bool _writeAll=false, \
               uint8_t _ExPin=-1, bool _RS232=false, \
               uint16_t _minRange_mm=501, uint16_t _maxRange_mm=4999);
		int16_t GetRange();
		String GetHeader();
		String GetString();

	private:

		uint8_t RxPin; // need not be global
		uint8_t nPings;
		bool writeAll;
		uint8_t ExPin;
		bool RS232;
		uint16_t minRange_mm;
		uint16_t maxRange_mm;

		uint16_t ranges[10] = {0}; //Fix hard code! and global??
		
    SoftwareSerial *softSerial;  //Fix hardcode!
		// extern SoftwareSerial softSerial;
    void serialBufferClear();
    int32_t sum(int16_t values[], uint8_t nvalues, bool errorNegative=true);
    float mean(int16_t values[], uint8_t nvalues, bool errorNegative=true);
    float standardDeviation(int16_t values[], uint8_t nvalues, float mean, \
                            bool errorNegative=true);
		
};

#endif
