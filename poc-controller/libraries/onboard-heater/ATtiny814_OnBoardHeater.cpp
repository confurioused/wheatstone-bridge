/* 
* ATtiny814_Heater.cpp
*
* Created: 12.12.2020 11:43:42
* Author: Lucas Nöller
*/


#include "ATtiny814_OnBoardHeater.h"

// default constructor
ATtiny814_OnBoardHeater::ATtiny814_OnBoardHeater()
{
} //ATtiny814_Heater

void ATtiny814_OnBoardHeater::setup(float lowerBorder, float upperBorder, bool isPortA, uint8_t pinNumber)
{
	this->setTargetTemperature(lowerBorder, upperBorder);
	this->isPortA = isPortA;
	this->pinNumber = pinNumber;
	
	if (this->isPortA)
	{
		PORTA.DIR |= 0x01 << this->pinNumber;	// configure as output
		PORTA.OUT &= ~(0x01 << this->pinNumber);	// define output as LOW
	} 
	else
	{
		PORTB.DIR |= 0x01 << this->pinNumber;	// configure as output
		PORTB.OUT &= ~(0x01 << this->pinNumber);	// define output as LOW
	}
}

void ATtiny814_OnBoardHeater::setTargetTemperature(float lowerBorder, float upperBorder)
{
	if (lowerBorder < ABSOLUTE_MIN_TEMPERATURE)
	{
		this->lowerThreshold = ABSOLUTE_MIN_TEMPERATURE;
	}
	else if (lowerBorder > ABSOLUTE_MAX_TEMPERATURE)
	{
		this->lowerThreshold = ABSOLUTE_MAX_TEMPERATURE;
	} 
	else
	{
		this->lowerThreshold = lowerBorder;
	}
	
	if (upperBorder < ABSOLUTE_MIN_TEMPERATURE)
	{
		this->upperThreshold = ABSOLUTE_MIN_TEMPERATURE;
	}
	else if (upperBorder > ABSOLUTE_MAX_TEMPERATURE)
	{
		this->upperThreshold = ABSOLUTE_MAX_TEMPERATURE;
	}
	else
	{
		this->upperThreshold = upperBorder;
	}
	if (this->upperThreshold < this->lowerThreshold)
	{
		this->upperThreshold = this->lowerThreshold;
	}
}

bool ATtiny814_OnBoardHeater::updateHeating(float currentTemperature)
{
	bool heatingState = false;
	if (currentTemperature < this->lowerThreshold)
	{
		heatingState = true;
		this->risingCycle = true;
	} 
	else if(currentTemperature < this->upperThreshold && this->risingCycle == true)
	{
		heatingState = true;
	}
	else
	{
		this->risingCycle = false;
	}

	if (this->enabled == false)
	{
		heatingState = false;
	}
	
	if (heatingState == true)
	{
		if (this->isPortA)
		{
			PORTA.OUT |= 0x01 << this->pinNumber;	// define output as HIGH
		} 
		else
		{
			PORTB.OUT |= 0x01 << this->pinNumber;	// define output as HIGH
		}
	} 
	else
	{
		if (this->isPortA)
		{
			PORTA.OUT &= ~(0x01 << this->pinNumber);	// define output as LOW
		}
		else
		{
			PORTB.OUT &= ~(0x01 << this->pinNumber);	// define output as LOW
		}
	}
	return heatingState;
}

float ATtiny814_OnBoardHeater::getLowerThreshold()
{
	return this->lowerThreshold;
}

float ATtiny814_OnBoardHeater::getUpperThreshold()
{
	return this->upperThreshold;
}

