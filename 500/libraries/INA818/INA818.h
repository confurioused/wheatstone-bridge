#pragma once

#include "../AD5258/AD5258.h"

#include "INA818_constants.h"

//forward declaration
class MessController;

/** Operational Amplifier
recommended use with AD5258 (50k) otherwise refer to table 2 of datasheet for constant gain
*/
class INA818{
	public:
		INA818(){};
		
		
		void init(MessController *MC, uint8_t gainSelect_addr);
		
		float getGain();
		/**
		discretized gain needed to amplify bridgevoltage to <2V !!! must not exceed 3.3V (AVDD ADC)
		*/
		void setGain(float gain);
		
	private: 
	AD5258 GainSelect;
	float currentGain;
	
	MessController *MessC;
};