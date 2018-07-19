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

Maxbotics::Maxbotics() 
{

}

uint8_t Maxbotix::begin() 
{
    //Initalize required peripherals
    //Allow for various models to be used? 
}

float Maxbotix::GetRange()  //will retrun distance to surface
{
  return 1701.0; //return dummy value until library is completed 
}

String Maxbotix::GetHeader()  //Add other stats if desired
{
  return "Distace [cm]";
}

String Maxbotix::GetString()
{
  return String(GetRange()) + ",";  //Return range as string
}
