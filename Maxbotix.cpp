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

uint8_t Maxbotix::begin(uint8_t _RxPin, uint8_t _nPings, bool _writeAll, \
                        uint8_t _ExPin, bool _RS232, \
                        uint16_t _minRange_mm, uint16_t _maxRange_m)
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
    if nPings == 0 {
      return false
    }
    else {
      return true
    }
}



int16_t Maxbotix::GetRange()  //will retrun distance to surface
{
  /**
   * @brief
   * Returns the result of a single range measurement
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
  if (ExPin >= 0){
    pinMode(Ex, OUTPUT);
    digitalWrite(Ex, HIGH);
    delay(1);
    digitalWrite(Ex, LOW);
  }
  
  // Get the 4 characters; if a carriage return is encountered, start fresh
  // from the beginning.
  // Time differences are unsigned; rollovers are a non-issue when differencing
  uint32_t start_time = millis();
  while ( (millis() - start_time) < timeout_millis ){
    if Serial.available(){
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
        rangeInt *= -1
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
  if (npings == 1){
    return "Distace [mm]";
  }
  else if (npings == 0){
    return "MAXBOTIX ERROR: SET NPINGS > 0";
  }
  else{
    String allPings = String("");
    if writeAll{
      for(int i=0; i<npings; i++)
      {
        allPings = String(allPings + "Distace [mm],");
      }
    }
    return String(allPings + \
                  "Mean Distace [mm],StDev Distance [mm],Error Count");
  }
}

String Maxbotix::GetString()
  /**
   * @brief
   * Returns the measurement result(s) as a String object
   *
   * @details
   * Returns the measurement result(s) as a String object
   *
   */
{
  return String(GetRange()) + ",";  //Return range as string
}


float Maxbotix::maxbotixHRXL_WR_Serial(uint8_t Ex, uint8_t npings, bool writeAll, \
            int maxRange, bool RS232){

  int myranges[npings]; // Declare an array to store the ranges [mm] // Should be int, but float for passing to fcns
  // Get nodata value - 5000 or 9999 based on logger max range (in meters)
  // I have also made 0 a nodata value, because it appears sometimes and shouldn't
  // (minimum range = 300 mm)
  int nodata_value;
  if (maxRange == 5){
    nodata_value = 5000;
  }
  else if (maxRange == 10){
    nodata_value = 9999;
  }
  // Put all of the range values in the array
  for (int i=0; i<npings; i++){
    // Must add int Rx to use this; currently, don't trust SoftwareSerial
    // myranges[i] = maxbotix_soft_Serial_parse(Ex, Rx, RS232);
    myranges[i] = maxbotix_Serial_parse(Ex);
  }
  // Then get the mean and standard deviation of all of the data
  int npings_with_nodata_returns = 0;
  unsigned long sum_of_good_ranges = 0;
  int good_ranges[npings];
  int j=0;
  for (int i=0; i<npings; i++){
    if (myranges[i] != nodata_value && myranges[i] != 0){
      sum_of_good_ranges += myranges[i];
      good_ranges[j] = myranges[i];
      j++;
    }
    else{
      npings_with_nodata_returns ++;
    }
  }
  float npings_with_real_returns = npings - npings_with_nodata_returns;
  float mean_range;
  float standard_deviation;
  // Avoid div0 errors
  if (npings_with_real_returns > 0){
    mean_range = sum_of_good_ranges / npings_with_real_returns;
    standard_deviation = standard_deviation_from_array(good_ranges, npings_with_real_returns, mean_range);
  }
  else {
    mean_range = -9999;
    standard_deviation = -9999;
  }
  // Write all values if so desired
  if (writeAll){
    for (int i=0; i<npings; i++){
      if (first_log_after_booting_up){
        headerfile.print("Ultrasonic distance to surface [mm]");
        headerfile.print(",");
        headerfile.sync();
      }
      datafile.print(myranges[i]);
      datafile.print(F(","));
      // Echo to serial
      Serial.print(myranges[i]);
      Serial.print(F(","));
    }

  }

  ///////////////
  // SAVE DATA //
  ///////////////

  if (first_log_after_booting_up){
    headerfile.print("Mean ultrasonic distance to surface [mm]");
    headerfile.print(",");
    headerfile.print("Standard deviation ultrasonic distance to surface [mm]");
    headerfile.print(",");
    headerfile.print("Number of readings with non-error returns");
    headerfile.print(",");
    headerfile.sync();
  }

  // Always write the mean, standard deviation, and number of good returns
  datafile.print(mean_range);
  datafile.print(F(","));
  datafile.print(standard_deviation);
  datafile.print(F(","));
  datafile.print(npings_with_real_returns);
  datafile.print(F(","));

  // Echo to serial
  Serial.print(mean_range);
  Serial.print(F(","));
  Serial.print(standard_deviation);
  Serial.print(F(","));
  Serial.print(npings_with_real_returns);
  Serial.print(F(","));

  // return mean range for functions that need it, e.g., to trigger camera
  return mean_range;
}

float ALog::standard_deviation_from_array(float values[], int nvalues,
            float mean){
  float sumsquares = 0;
  for (int i=0; i<nvalues; i++){
    sumsquares += square(values[i] - mean);
  }
  return sqrt(sumsquares/nvalues);
}

float ALog::standard_deviation_from_array(int values[], int nvalues,
            float mean){
  float sumsquares = 0;
  for (int i=0; i<nvalues; i++){
    sumsquares += square(values[i] - mean);
  }
  return sqrt(sumsquares/nvalues);
}

int ALog::maxbotix_Serial_parse(uint8_t Ex){
  // NOTE: Currently assumes only one Serial port.
  // Excites the MaxBotix sensor and receives its ranging output
  char range[7]; // R####<\r>, so R + 4 chars + carriage return + null
  Serial.end(); // End 38400 bps computer comms
  Serial.begin(9600); // Start 9600 bps logger comms
  //Excite the sensor to produce a pulse
  pinMode(Ex, OUTPUT);
  digitalWrite(Ex, HIGH);
  delay(1);
  digitalWrite(Ex, LOW);
//  delay(150); //Chad, do I need a 150ms delay needed to make sure low at end of sample to get unfiltered readings?
  // Record the result of the ranging
  int i=0; // counter
  // Not sure if this will work - maybe loop around to the other end of the array?
  while (range[i-1] != 13){
    if (Serial.available()){
      range[i] = Serial.read();
      i++;
    }
  }
  Serial.end();
  Serial.begin(38400);
  // Convert to integer
  char r2[4]; // range stripped of all of its extra characters
  for (int i=1; i<5; i++){
    r2[i-1] = range[i];
  }
  int r3 = atol(r2);
  return r3;
}

///////////////////////
// PRIVATE FUNCTIONS //
///////////////////////

void serialBufferClear(){
  while(Serial.available()) {
    Serial.read();
  }


}   
