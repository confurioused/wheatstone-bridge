#include "ADS112C04I.h"

#include "../MessController/MessController.h"

 void ADS112C04I::init(uint8_t twi_addr, MessController *MC){
		
		this->TWI_ADDR = twi_addr;
		this->MessC = MC;
		//reset to default values 
		MessC->write8BitCommand(TWI_ADDR,COMMAND_RESET);
		
		//set ADC Mux to AIN3 
		
		MessC->writeData(TWI_ADDR,COMMAND_WREG, INPUTPORT);
		
		//(TODO: measuring supply AVDD/4 via ADC MUX 1101)
}

void ADS112C04I::requestSample(){
	MessC->write8BitCommand(TWI_ADDR, COMMAND_START);
}

uint16_t ADS112C04I::getSample(){
	
	return MessC->readData16Bit(TWI_ADDR,COMMAND_RDATA) *  Vref / getGain() / ADC_RESOLUTION; //single ended
	
}


GAIN ADS112C04I::getGain(){
	return currentGain;
}


void ADS112C04I::setGain(GAIN newGain){
	currentGain = newGain;
	uint8_t temp = MessC->readData(TWI_ADDR,0x00) & (~0x07 << 1);
	uint8_t newGainbm = temp || newGain<<1;
	MessC->writeData(TWI_ADDR,0x00,newGainbm);
}