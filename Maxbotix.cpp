/******************************************************************************
Maxbotix.cpp
Library for interfacing with Maxbotix rangefinders for use with enviromental sensing
Bobby Schulz @ Northern Widget LLC
Andy Wickert @ Northern Widget LLC
7/16/2018
https://github.com/NorthernWidget-Skunkworks/Maxbotix_Library

This script is used to interface to the Maxbotix sensors and return distance to target, along with other
statistics

Boss: "I just heard that light travels faster than sound. I'm wondering if I should shout when I speak, just so my lips appear to sync-up with my words.""
Dilbert (thought): "A little knowledge can be a ridiculous thing."
-Scott Adams

Distributed as-is; no warranty is given.
******************************************************************************/

#include "Maxbotix.h"

Maxbotix::Maxbotix() 
{
}

bool Maxbotix::begin(uint8_t _RxPin, uint8_t _nPings, bool _writeAll, \
                        uint8_t _ExPin, bool _RS232, \
                        uint16_t _minRange_mm, uint16_t _maxRange_mm)
{
  /**
   * @brief
   * Sets globals and initializes software serial
   *
   * @details
   * Sets global variables required for a SoftwareSerial interface to record 
   * data from a MaxBotix ultrasonic rangefinder. Initializes software serial 
   * based on _RxPin.
   *
   * @param _RxPin Pin for SoftwareSerial receive at 9600 bps.
   *
   * @param _npings Number of pings over which you average; each ping itself
   * includes ten short readings that the sensor internally processes.
   * Must be > 0, obviously, and <= 255. Defaults to "1"
   *
   * @param _writeAll will write each reading of the sensor (each ping)
   * to the serial monitor and SD card. This is relevant only if npings > 1
   *
   * @param _ExPin Excitation pin that turns the sensor on; defaults to "-1",
   * assuming that the sensor is either always on or is being switched via
   * its main power supply.
   *
   * @param _RS232 defaults false for standard (TTL) logic; true for inverse
   * (RS232-style) logic. It works at standard logger voltages: this is not
   *  true RS232, but this is what MaxBotix calls it.
   *
   * @param _minRange_mm Minimum sensor range in mm; defaults to 501
   *
   * @param _maxRange_mm Maximum sensor range in mm; defaults to 4999
   *
   * Example:
   * ```
   * // SoftwareSerial with RxPin 7, averaging over 10 pings, and otherwise
   * // using default settings
   * alog.maxbotixHRXL_WR_Serial(7, 10);
   * ```
   *
   */
    
    RxPin = _RxPin;
    nPings = _nPings;
    writeAll = _writeAll;
    ExPin = _ExPin;
    RS232 = _RS232;
    minRange_mm = _minRange_mm;
    maxRange_mm = _maxRange_mm;

    // Not sure if this will work
    softSerial = new SoftwareSerial(RxPin, -1);
    
    // Test if npings is in the proper range
    if(nPings == 0){
      return false;
      nPings = 1;
    }
    else {
      return true;
    }
}



int16_t Maxbotix::GetRange()  //will retrun distance to surface
{
  /**
   * @brief
   * Returns the result of a single range measurement -- now set for multiple?
   *
   * @details
   * Returns the result of a single range measurement.
   * Communications error value = -9999
   * Internal error value (including range too long) = 5000
   * Range too short error value = 500
   * This code makes these internal error values negative to more easily
   * sort them out of the real results
   *
   */
  static uint16_t ranges[nPings];

  softSerial.begin(9600);

  // Input range is in the format R####<\r>
  // R + 4 chars + carriage return + null = 7
  // I will accept only ASCII numeric values, so will shorten this to 5
  // 4 chars between 48 and 57 (inclusive) + null
  char range[5];
  char _tmpchar;
  bool success_flag = false;
  int16_t rangeInt;
  // counter
  uint8_t i=0;
  // Timeout: this should be long enough for 4 readings, and should be
  // triggered iff something has gone really wrong
  uint8_t timeout_millis = 20;

  // Remove junk from the serial line -- this may be a lot if there is no
  // excitation applied and power has been on for a while
  serialBufferClear();

  //Excite the sensor to produce a pulse, if you have selected to do so.
  if (RxPin >= 0){
    pinMode(RxPin, OUTPUT);
    digitalWrite(RxPin, HIGH);
    delay(1);
    digitalWrite(RxPin, LOW);
  }
  
  // Get the 4 characters; if a carriage return is encountered, start fresh
  // from the beginning.
  // Time differences are unsigned; rollovers are a non-issue when differencing
  uint32_t start_time = millis();
  while ( (millis() - start_time) < timeout_millis ){
    if(Serial.available()){
      _tmpchar = Serial.read();
      // Test if ASCII number
      if ( (_tmpchar >= 48) && (_tmpchar <= 57) ){
        range[i] = _tmpchar;
        i += 1;
      }
      else if (_tmpchar == 13){
        i = 0; // reset to the start of a measurement
      }
    }
    // Break if enough characters are recorded
    if (i == 4){
      success_flag = true;
      break;
    }
  }

  softSerial.end();

  // Double-check that the NULL will not cause a problem here
  if (success_flag){
     rangeInt = atoi(range);
     // negative if an error value
     if ( (rangeInt == 5000) || (rangeInt == 500) ) {
        rangeInt *= -1;
     }
  }
  else{
    return -9999; // hard-coded communications error value
  }
}

String Maxbotix::GetHeader()
{
  /**
   * @brief
   * Creates the appropritae header for the data file
   *
   * @details
   * Creates the appropritae header for the data file, based on
   * provided inputs (number of pings, recording all pings)
   *
   */
  if (nPings == 1){
    return "Distace [mm]";
  }
  else if (nPings == 0){
    return "MAXBOTIX ERROR: SET NPINGS > 0.";
  }
  else{
    String allPings = String("");
    if(writeAll){
      for(int i=0; i<nPings; i++)
      {
        allPings = String(allPings + "Distace [mm],");
      }
    }
    return String(allPings + \
                  "Mean Distace [mm],StDev Distance [mm],Error Count");
  }
}

String Maxbotix::GetString(){
  /**
   * @brief
   * Returns the measurement result(s) as a String object
   *
   * @details
   * Returns the measurement result(s) as a String object
   *
   */
  String outstr;
  if (nPings == 1){
    outstr = String(GetRange()) + ",";  //Return range as string
  }
  else if (nPings == 0){
    outstr = "NO PINGS REQUESTED,";
  }
  else{
    outstr = "";
    if(writeAll){
      for(int i=0; i<nPings; i++)
      {
        outstr = String( outstr + String(ranges[i]) + "," ) ;
      }
    }
    float _mean = mean(ranges, nPings);
    float _SD = standardDeviation(ranges, nPings, _mean);
    uint8_t _nerr = 0;
    for(int i=0; i<nPings; i++)
    {
      if (ranges[i] < 0){
        _nerr += 1;
      }
    }
    outstr = String( outstr + String(_mean) + "," );
    outstr = String( outstr + String(_SD) + "," );
    outstr = String( outstr + String(_nerr));
  }
  return outstr;
}

///////////////////////
// PRIVATE FUNCTIONS //
///////////////////////

void Maxbotix::serialBufferClear(){
  while(Serial.available()) {
    Serial.read();
  }
}

int32_t Maxbotix::sum(int16_t values[], uint8_t nvalues, \
                       bool errorNegative){
  uint32_t _sum = 0;
  for (int i=0; i<nvalues; i++){
    if ( (errorNegative == false) || (values[i] >= 0) ){
      _sum += values[i];
    }
  return _sum;
  }
}

float Maxbotix::mean(int16_t values[], uint8_t nvalues, \
                     bool errorNegative){
  uint32_t _sum = 0;
  float nvalues_valid = 0.;
  float mean;
  for (int i=0; i<nvalues; i++){
    if ( (errorNegative == false) || (values[i] >= 0) ){
      _sum += values[i];
      nvalues_valid += 1.;
    }
  }
  // how to handle if nvalues_valid = 0? Will it throw an error?
  mean = _sum/nvalues_valid;
  return mean;
}

float Maxbotix::standardDeviation(int16_t values[], uint8_t nvalues, \
                                  float mean, bool errorNegative){
  float sumsquares = 0;
  float nvalues_valid = 0.;
  for (int i=0; i<nvalues; i++){
    if ( (errorNegative == false) || (values[i] >= 0) ){
      sumsquares += square(values[i] - mean);
      nvalues_valid += 1.;
    }
  }
  return sqrt(sumsquares/nvalues_valid);
}

