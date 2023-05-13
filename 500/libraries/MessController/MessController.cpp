#include "MessController.h"

//10.09.22: bridge equation + system for saving measurement data


void MessController::init(uint8_t address) {
	
	AtTinyTWISlave::init(address, 16);        //Set address and number of TWI registers

	
	
	//set pin B2 Interrupt enable
	PORTB.PIN2CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	getPoti1().init(Poti1Addr, this, Poti1R_AB, Poti1R_WB);
	getADC().init(ADCAddr, this);
	setPoti(0);//TODO 0 not possible 
	
	OpAmp.init(this, gainSelect_addr);
	
	//TODO allocate Datafield dependant on inputconfigs
	
}


void MessController::handle() {
		
	
			
		/*
		taking measurements: (isMaster)
		
		1.set MUC 
		2. bridge balancing
		3. read and store ADC results
		
		idle for pi readings: (!isMaster)
		*/
		
	AtTinyTWIMaster::setMasterStatus(true);
				
		
	
	for(int i= 0; i<12;i++){//for every input 
		if(SingleInputconfigs[i]){
			//smallest gain so that no overvoltage on ADC
			ADC.setGain(GAIN_1);
			OpAmp.setGain(MinGain); 
			
			setMuxSingle(i+1); //Ports start with M1
			
			balanceBridgeSingle();
			
			measurements[0] = ADC.getSample(); //TODO: system for organizing measurements
		}	
	}		
}


void MessController::commHandler() {
	//confirm silence on internal - bus busidle SSTATUS.BUSSTATE == 0x1 TODO
	AtTinyTWIMaster::setMasterStatus(false);
	MCReady.set(true);
	
	
}


void MessController::interruptHandler() {
	//let pi read data
	
	AtTinyTWISlave::interruptHandler();        //Execute standard TWI interrupt handler
	
	
	AtTinyTWIMaster::setMasterStatus(true);
	
}

void MessController::setMuxDouble(unsigned char IC1_Port, unsigned char IC2_Port){
	//TODO
}

void MessController::setMuxSingle(unsigned char inputPort, unsigned char offsetPort){
		
		switch(inputPort){
			case 1: 
				MUX1A2.set(true);
				MUX1A1.set(true);
				MUX1A0.set(true);
				break;
				
			case 2:
				MUX1A2.set(true);
				MUX1A1.set(true);
				MUX1A0.set(false);
				break;
			
			case 3:
				MUX1A2.set(true);
				MUX1A1.set(false);
				MUX1A0.set(true);
				break;
			
			case 4:
				MUX1A2.set(false);
				MUX1A1.set(true);
				MUX1A0.set(true);
				break;
			
			case 5:
				MUX1A2.set(false);
				MUX1A1.set(true);
				MUX1A0.set(false);
				break;
			
			case 6:
				MUX1A2.set(false);
				MUX1A1.set(false);
				MUX1A0.set(true);
				break;
				
			default:
				MUX1A2.set(false);
				MUX1A1.set(false);
				MUX1A0.set(false);
				break;
					
		}
		switch(offsetPort){
			case 7:
				MUX2A2.set(true);
				MUX2A1.set(true);
				MUX2A0.set(false);
				break;
			case 8:
				MUX2A2.set(true);
				MUX2A1.set(true);
				MUX2A0.set(true);
				break;
			case 9:
				MUX2A2.set(false);
				MUX2A1.set(true);
				MUX2A0.set(true);
				break;
			case 10:
				MUX2A2.set(false);
				MUX2A1.set(true);
				MUX2A0.set(false);
				break;
			case 11:
				MUX2A2.set(false);
				MUX2A1.set(false);
				MUX2A0.set(true);
				break;
			case 12:
				MUX2A2.set(false);
				MUX2A1.set(false);
				MUX2A0.set(false);
				break;
				
			default:
				MUX2A2.set(true);
				MUX2A1.set(false);
				MUX2A0.set(true);
				break;
		}
}
void MessController::balanceBridgeSingle(){
	float measureResi;
	float newPotiResi;

	float bridgeVoltage;
	
	do{
		/*TODO:
		set ADC reference to 3.3V
		measure voltage
		set poti --> bridge to 0+ (15 Ohms accurate for testing)
		(set gain to 
		measure voltage
		set ADC reference to 2V 
		set gain --> 2V
		*/
		
		//discretized gain needed to amplify bridgevoltage to <2V !!! must not exceed 3.3V (AVDD ADC)
		
		bridgeVoltage = getBridgeVoltage();
		
		if(bridgeVoltage<Vref){
			OpAmp.setGain(Vref/bridgeVoltage);
		}
		else{
			//TODO: does this avoid danger for adc 3.3V?
			ADC.setGain(GAIN_1);
			OpAmp.setGain(MinGain);
		}
		measureResi = voltageToMeasureResi(bridgeVoltage);
		newPotiResi = measureResiToPoti(measureResi);
		setPoti(newPotiResi);
		
	}while(bridgeVoltage>THRESHOLD);
}


void MessController::setPoti(float potiResi){
	
	
	Poti1.setResistance(potiResi);
	//TODO: finetuning of other Potentiometers
}


float MessController::getBridgeVoltage(){
	ADC.requestSample();
	while(DRDY.get() == 0){
		//waiting for data to be ready 
	}
	return ADC.getSample()/ OpAmp.getGain();
}

float MessController::voltageToMeasureResi(float bridgeVoltage){
	
	//TODO: real measured equation
	return 0;
}

float MessController::measureResiToPoti(float measureResi){
	//solve bridge equation
	//TODO: real measured equation
	return 0;
}

AD5258 MessController::getPoti1(){
	return this->Poti1;
}
ADS112C04I MessController::getADC(){
	return this->ADC;
}