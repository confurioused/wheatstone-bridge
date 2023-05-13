#include "INA818.h"

void INA818::init(MessController *MC, uint8_t gainSelect_addr){
	GainSelect.init(gainSelect_addr, MC, RGainSelect);
}



float INA818::getGain(){
	return currentGain;
}


void INA818::setGain(float gain){
	GainSelect.setResistance(RGainSelect/(gain-1));
};


