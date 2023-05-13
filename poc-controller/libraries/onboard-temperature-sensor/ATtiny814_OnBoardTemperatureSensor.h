/* 
* ATtiny814_OnBoardTemperatureSensor.h
*
* Created: 12.12.2020 11:44:51
* Author: Lucas Nöller
*/


#ifndef __POC_ONBOARDTEMPERATURESENSOR_H__
#define __POC_ONBOARDTEMPERATURESENSOR_H__

#include "../adc/ATtiny814_ADC.h"

class ATtiny814_OnBoardTemperatureSensor
{
//variables
public:
	float slopeCalibration;
	float offsetCalibration;
	float currentTemperature = 0;	// last measured temperature [°C]
private:
	ATtiny814_ADC currentADC;

//functions
public:
	/*
		General constructor of this class.
	*/
	ATtiny814_OnBoardTemperatureSensor();

	/*
		Set up specific temperature sensor object with calibration values to calculate temperature in °C from read voltage. Also starts ADC conversion
		Parameters:
			- input : Channel - ADC to read data from for this temperature sensor
			- [optional] slopeCalibration : float - Slope of linear regression function to calculate temperature in °C from sensor voltage output
					standard value is 100
			- [optional] offsetCalibration : float - Offset of linear regression function to calculate temperature in °C from sensor voltage output
					standard value is 0.5
	*/
	void setup(Channel input, float slopeCalibration = 100, float offsetCalibration = 50);

	/*
		Returns temperature in °C of the corresponding temperature sensor.
		Return:
		 - float - Temperature [°C] of temperature sensor calculated by: realTemp = (measuredVoltage - offsetCalibration) / slopeCalibration
	*/
	float getTemperature();

private:

}; //ATtiny814_OnBoardTemperatureSensor

#endif //__POC_ONBOARDTEMPERATURESENSOR_H__
