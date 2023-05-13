#pragma once
		
#include "AtTinyTWISlave.h"
#include "libraries/twi/Pin.h"

class MessController : public AtTinyTWISlave{
	public:
	virtual void init(uint8_t address) {
		AtTinyTWISlave::init(address, 16);        //Set address and number of TWI registers
		//get initial inputs	
		
	}

	virtual void handle() {
		
		/*
		taking measurements: (isMaster)
		1.set MUC 
		2. bridge balancing
		3. read and store ADC results
		
		idle for pi readings: (!isMaster)
		
		if(stopConditionReceived()){
			isMaster= true;
		}
		*/
		
		//getRegister(11);                        //read register 11

		//setRegister(12, 0x42);                    //write '0x42' to register 12
	}

	virtual void interruptHandler() {
		//let pi read data
		AtTinyTWISlave::interruptHandler();        //Execute standard TWI interrupt handler
		
		/*
		isMaster = true;
		*/
	}
	void commHandler() {
		/*
		1. confirm silence on internal bus
		2. isMaster = false;
		3. signal readiness via interrupt pin 7
		
		*/
	}
	private:
	enum RESISTANCE_INPUTPINS{
		/*
	M1 - MUX1 (IC12 in KICAD) Pin 9 
	M2 - MUX1 (IC12 in KICAD) Pin 10
	M3 - MUX1 (IC12 in KICAD) Pin 11
	M4 - MUX1 (IC12 in KICAD) Pin 7
	M5 - MUX1 (IC12 in KICAD) Pin 6
	M6 - MUX1 (IC12 in KICAD) Pin 5
	M7 - MUX2 (IC12 in KICAD) Pin 10
	M8 - MUX2 (IC12 in KICAD) Pin 9
	M9 - MUX2 (IC12 in KICAD) Pin 7
	M10 - MUX2 (IC12 in KICAD) Pin 6
	M11 - MUX2 (IC12 in KICAD) Pin 5
	M12 - MUX2 (IC12 in KICAD) Pin 4*/
		};
};