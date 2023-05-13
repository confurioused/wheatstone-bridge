/* 
* ATtiny814_Heater.h
*
* Created: 12.12.2020 11:43:42
* Author: Lucas Nöller
*/


#ifndef __ATTINY814_HEATER_H__
#define __ATTINY814_HEATER_H__

#define ABSOLUTE_MIN_TEMPERATURE (-70.0)
#define ABSOLUTE_MAX_TEMPERATURE (50.0)

#include <avr/io.h>

class ATtiny814_OnBoardHeater
{
//variables
public:
	bool enabled = false;

private:
	float lowerThreshold = 0;	// lower limit of the controller in °C
	float upperThreshold = 10;	// upper limit of the controller in °C
	bool risingCycle = true;	// determines if the temperature is supposed to cool-down until lowerThreshold is reached (false) or if it has to rise until upperThreshold

	// used to determine the pin connected to the heater
	bool isPortA;	// port of the pin
	uint8_t pinNumber;	// number of the pin in the datasheet

//functions
public:
	/*
		Generic constructor for this class
	*/
	ATtiny814_OnBoardHeater();

	/*
		Initialize object and pins to control heating cycles. Also calls setTargetTemperature() on both parameters
		Parameter:
			- lowerThreshold : float - Temperature at which the heater starts heating again
			- upperThreshold : float - Temperature at which the heater stops heating
			- isPortA : bool - True if pin is part of PortA (PAx) and false if it is part of PortB (PBx) (see data sheet page 16)
			- pinNumber : uint8_t - Number of pin according to data sheet (PAx -> pinNumber = x)
	*/
	void setup(float lowerThreshold, float upperThreshold, bool isPortA, uint8_t pinNumber);

	/*
		Sets temperature set-points for 2-point controller. Controller will hold temperature within these boundaries.
		Will automatically check out of boundary values [-70 and +50].
		Parameter:
			- lowerThreshold : float - Temperature at which the heater starts heating again
			- upperThreshold : float - Temperature at which the heater stops heating
	*/
	void setTargetTemperature(float lowerThreshold, float upperThreshold);

	/*
		Feeds the current module temperature to the algorithm to determine if the heater will run during this cycle (2-point controller used).
		Paramter:
			- currentTemperature : float - Temperature of the module [°C]
		Return:
			- bool - True if heating during upcoming cycle, False of not
	*/
	bool updateHeating(float currentTemperature);

	/*
		Function returns the current lower threshold.
		Return:
			- float - Current lower threshold temperature [°C]
	*/
	float getLowerThreshold();

	/*
		Function returns the current upper threshold.
		Return:
			- float - Current upper threshold temperature [°C]
	*/
	float getUpperThreshold();

private:

}; //ATtiny814_Heater

#endif //__ATTINY814_HEATER_H__
