#pragma once
#include "../twi/PoCController.h"
#include "Pin.h"
class POCController500 : PoCController{
	private:
	Pin MC_signal_in {PORT_t* const &PORTB, const uint8_t 2, bool false};
	Pin MC_signal_out {PORT_t* const &PORTA, const uint8_t 4, bool true};
	public:
	
	
	void interruptHandler() {
		//waiting until Messcontroller is ready (passively clockstreching) 
	
		if(stopConditionReceived() && reg_number_received) {			//Enable/Disable TWI/SPI only after a Stop-Condition has been received
			switch(current_register) {
				case 0x00:
				//signal for datarequest pi --> silence on local bus request
				MC_signal_out.set(true);
				//polling for signal from Messcontroller (local bus idle)
				while( MC_signal_in.get() == false){
					//do nothing
				}
				MC_signal_out.set(false);
				
				activateTWI();
				break;
			
				case 0xFE:
				deactivateTWI();
				break;
			
				case 0xFD:				//Not official yet
				deactivateSPI();
				break;
			}
		
			if(current_register >= 1 && current_register <= 16) {			//writing registers 1 to 16 enables the corresponding SPI chip select
				activateSPI();
				activateSPIComponent(current_register - 1);			//first SPI Component has Index 0 but slotNr 1
			}
		
			reg_number_received = false;
		}
		AtTinyTWISlave::interruptHandler();
	};
		
	
};
	

	