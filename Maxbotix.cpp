/******************************************************************************
Maxbotics.cpp
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

#include "Maxbotics.h"

Maxbotics::Maxbotics() 
{

}

uint8_t Maxbotics::begin() 
{
    //Initalize required peripherals
    //Allow for various models to be used? 
}

float Maxbotics::GetRange()  //will retrun distance to surface
{
  return 1701.0; //return dummy value until library is completed 
}

String Maxbotics::GetHeader()  //Add other stats if desired
{
  return "Distace [cm]";
}

String Maxbotics::GetString()
{
  return String(GetRange()) + ",";  //Return range as string
}