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

uint8_t Maxbotix::begin(uint8_t _RxPin, uint8_t _npings, bool _writeAll, \
                        uint8_t _ExPin, bool _RS232, \
                        uint16_t _minRange_mm, uint16_t _maxRange_m)
{
  /**
   * @brief
   * Sets globals.
   *
   * @details
   * Sets global variables required for a SoftwareSerial interface to record 
   * data from a MaxBotix ultrasonic rangefinder.
   *
   * @param _RxPin Pin for SoftwareSerial receive at 1200 bps.
   *
   * @param _npings Number of pings over which you average; each ping itself
   * includes ten short readings that the sensor internally processes.
   * Must be > 0, obviously. Defaults to "1"
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
   * @param _minRange_mm Minimum sensor range in mm; defaults to 500
   *
   * @param _maxRange_mm Maximum sensor range in mm; defaults to 5000
   *
   * Example:
   * ```
   * // SoftwareSerial on , averaging over 10 pings,
   * // not recording the results of each ping, and with a maximum range of
   * // 5000 mm using standard TTL logic
   * alog.maxbotixHRXL_WR_Serial(7, 10, false, 5000, false);
   *
   * ```
   */

    RxPin = _RxPin;
    npings = _npings;
    writeAll = _writeAll;
    ExPin = _ExPin;
    RS232 = _RS232;
    minRange_mm = _minRange_mm;
    maxRange_mm = _maxRange_mm;
}



float Maxbotix::GetRange()  //will retrun distance to surface
{
  /**
   * @brief
   * Returns the result of a single range measurement
   *
   * @details
   * Returns the result of a single range measurement
   * @param _RxPin Pin for SoftwareSerial receive at 1200 bps.
   *
   * @param _npings Number of pings over which you average; each ping itself
   * includes ten short readings that the sensor internally processes.
   * Must be > 0, obviously. Defaults to "1"
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
   * @param _minRange_mm Minimum sensor range in mm; defaults to 500
   *
   * @param _maxRange_mm Maximum sensor range in mm; defaults to 5000
   *
   * Example:
   * ```
   * // Digital pin 7 controlling sensor excitation, averaging over 10 pings,
   * // not recording the results of each ping, and with a maximum range of
   * // 5000 mm using standard TTL logic
   * alog.maxbotixHRXL_WR_Serial(7, 10, false, 5000, false);
   *
   * ```
   */
  return 1701.0; //return dummy value until library is completed 
}

String Maxbotix::GetHeader()  //Add other stats if desired
{
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
    return String(allPings + "Mean Distace [mm],StDev Distance [mm]");
  }
}

String Maxbotix::GetString()
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
