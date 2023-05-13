#pragma once


#include "stdint.h"
#include "avr/io.h"

#include "AD5258_constants.h"

//forward declaration
class MessController;

class AD5258{
	
	public:
		AD5258(){};
		
		/**
		@param	R_AB - nominal resistance between Terminal A and B. i.e. 1k Ohm, 50 KOhm
				R_WB - external resistor between wiper and terminal B used shorten discrete stepsize
		*/ 
		void init(uint8_t twi_addr, MessController *MC,float R_AB, float R_WB = 0);
		
		void setResistance(float desired_resistance);
		
		
		float getR_AB();
		
		float getR_WB();
		
		uint8_t getTWI_ADDR();
		
		float getMaxResistance();
		
		float getMinResistance();
		
	private:
		uint8_t TWI_ADDR;
		MessController *MessC;
		float R_AB;
		float R_WB;
		
		//TODO: measure real maximum
		float max_resistance = 2 * Rw + R_AB;
		float min_resistance = 2 * Rw + R_AB/64;
		/**
			wiperposition d = [0,64] sets resistace
		*/
		void setWiperPos(int d);
		
};