#pragma once
#include "ADS112C04I_constants.h"
#include "stdint.h"

//forward declaration
class MessController;

class ADS112C04I{
	
	 public:
	 
		ADS112C04I(){};
		
	 	void init(uint8_t twi_addr, MessController *MC);
 		
		void requestSample();
		
		/**
		returns 16 bit voltage connected to inputport
		*/
		uint16_t getSample();
		
		GAIN getGain();
	  
		void setGain(GAIN newGain);
		
	private:
		uint8_t TWI_ADDR;
		MessController *MessC;
		GAIN currentGain;
 };