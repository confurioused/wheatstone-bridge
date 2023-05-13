#pragma once
		
#include "../twi/AtTinyTWISlave.h"
#include "../twi/AtTinyTWIMaster.h"
#include "../twi/Pin.h"

#include "../ADS112C04I/ADS112C04I.h"
#include "../INA818/INA818.h"
#include "../AD5258/AD5258.h"
#include "MessController_constants.h"
#include "math.h"



class MessController : public AtTinyTWISlave, public AtTinyTWIMaster{
	public:
		MessController(){};
		
		void init(uint8_t address); 
	
		void handle() override;
	
		void commHandler();
	
		void interruptHandler();
		
		

	private:
	
		ADS112C04I ADC;
		INA818 OpAmp;
		AD5258 Poti1; //biggest
		//AD5258 Poti2; //medium
		//AD5258 Poti3; //smallest
		
		uint16_t measurements[measurementFieldSize] = {}; //TODO: how much space availible? malloc to save in heap?
		
		
		//incomming interrupt pin 7 (MCgetReady)
		Pin MCgetReady {&PORTB, 2, false};
		//dataready as inputport pin 5
		Pin DRDY{&PORTA, 7, false};
		//outgoing interrupt pin 6
		Pin MCReady {&PORTB, 3, true};
		
		//output pins mux a0,a1,a2
		Pin MUX1A0 {&PORTA, 6, true};
		Pin MUX1A1 {&PORTA, 4, true};
		Pin MUX1A2 {&PORTA, 5, true};
		Pin MUX2A0 {&PORTA, 1, true};
		Pin MUX2A1 {&PORTA, 2, true};
		Pin MUX2A2 {&PORTA, 3, true};
	
	
		/**
			translation between desired input source and mux control
			[A2,A1,A0]
			mit MUX2 S6 --> 101
			M1 - MUX1 (IC12 in KICAD) S8 --> 111
			M2 - MUX1 (IC12 in KICAD) S7 --> 110
			M3 - MUX1 (IC12 in KICAD) S6 --> 101
			M4 - MUX1 (IC12 in KICAD) S4 --> 011
			M5 - MUX1 (IC12 in KICAD) S3 --> 010
			M6 - MUX1 (IC12 in KICAD) S2 --> 001
		
			mit MUX1 S1 --> 000
			M7 - MUX2 (IC14 in KICAD) S7 --> 110
			M8 - MUX2 (IC14 in KICAD) S8 --> 111
			M9 - MUX2 (IC14 in KICAD) S4 --> 011
			M10 - MUX2 (IC14 in KICAD) S3 --> 010
			M11 - MUX2 (IC14 in KICAD) S2 ---> 001
			M12 - MUX2 (IC14 in KICAD) S1 --> 000
		*/
	
		/**
			wheatstone half bridge
			
		*/
		void setMuxDouble(unsigned char IC1_Port, unsigned char IC2_Port);
		
		/**
			inputPort[1,6]
			offsetPort[7,12]
			in default the difference between MUX and other resistances will be measured 
		*/
		void setMuxSingle(unsigned char inputPort = 0, unsigned char offsetPort = 0);
	
		/**
			Wheatstone bridge voltage needs to be driven to 0V (input offset 4uV)
			for measurement 2V after gain
		*/
		void balanceBridgeSingle();
	
		/**
			set the potentiometer resistance for all three IC's combined
			testversion: only Poti1
		*/
		void setPoti(float potiResi);
	
	
		float getBridgeVoltage();
	
		/**
			calculates the resistance in the Wheatstone-bridge based on the voltage across the bridge
		*/
		float voltageToMeasureResi(float bridgeVoltage);
	
		/**
			calculates the resistance the potentiometer-circuit needs to have to drive the bridgevoltage to zero
		*/
		float measureResiToPoti(float measureResi);
		
		AD5258 getPoti1();
		ADS112C04I getADC();
};