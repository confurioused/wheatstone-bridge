#include "AD5258.h"

#include "../MessController/MessController.h"



void AD5258::init(uint8_t twi_addr, MessController *MC, float R_AB, float R_WB){
	this->TWI_ADDR = twi_addr;
	this->MessC = MC;
	this->R_AB = R_AB;
	this->R_WB = R_WB;
	
	setWiperPos(0);
	//TODO: deviation from measured values?
}



void AD5258::setResistance(float desired_resistance){
	if(desired_resistance < min_resistance){
		desired_resistance = min_resistance;
	}
	if(desired_resistance > max_resistance){
		desired_resistance = max_resistance;
	}
	
	if(R_WB == 0){
		setWiperPos((int)-((desired_resistance - 2 * Rw)* 64 / R_AB - 64));
	}
	else{
		setWiperPos((int)(sqrt(R_AB-desired_resistance+2*Rw)*sqrt(R_AB+4*R_WB-desired_resistance+6*Rw)+R_AB-desired_resistance)*64/(2 * R_AB)); //TODO: find actual measured behaviour of Poti
	}
	
}


float AD5258::getR_AB(){
	return R_AB;
}


float AD5258::getR_WB(){
	return R_WB;
}


uint8_t AD5258::getTWI_ADDR(){
	return TWI_ADDR;
}



void AD5258::setWiperPos(int d){
	
	MessC->writeData(TWI_ADDR,0x00,d);
	
}

float AD5258::getMaxResistance(){
	return max_resistance;
}

float AD5258::getMinResistance(){
	return min_resistance;
}