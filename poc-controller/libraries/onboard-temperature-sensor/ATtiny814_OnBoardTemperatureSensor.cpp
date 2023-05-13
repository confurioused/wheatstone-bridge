/* 
* ATtiny814_OnBoardTemperatureSensor.cpp
*
* Created: 12.12.2020 11:44:51
* Author: Lucas Nöller
*/


#include "ATtiny814_OnBoardTemperatureSensor.h"

// default constructor
ATtiny814_OnBoardTemperatureSensor::ATtiny814_OnBoardTemperatureSensor()
{
} //ATtiny814_OnBoardTemperatureSensor


void ATtiny814_OnBoardTemperatureSensor::setup(Channel input, float slopeCalibration /*= 100*/, float offsetCalibration /*= 50*/)
{
	this->slopeCalibration = slopeCalibration;
	this->offsetCalibration = offsetCalibration;
	this->currentADC = ATtiny814_ADC(input, RunMode::FREERUN, Resolution::BIT_10, Prescaler::DIV256, ReferenceVoltage::INTERNAL, Accumulation::ACC4);
}

float ATtiny814_OnBoardTemperatureSensor::getTemperature()
{
	this->currentTemperature = (this->currentADC.readValue() - this->offsetCalibration) / this->slopeCalibration;
	return this->currentTemperature;
}
