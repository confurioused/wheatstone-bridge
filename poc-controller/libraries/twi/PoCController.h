/* 
* PoCController.h
*
* Created: 22.05.2021 19:40:44
* Author: Benjamin
*/


#ifndef __POCCONTROLLER_H__
#define __POCCONTROLLER_H__

#include "stdint.h"
#include <avr/interrupt.h>
#include "Pin.h"
#include "AtTinyTWISlave.h"

class PoCController : public AtTinyTWISlave
{
public:	
	Pin twi_disable {&PORTB, 3};
	Pin led_enable {&PORTA, 5};
	Pin spi_disable {&PORTB, 3};
	Pin* csn_disable;
	Pin* active_cs = nullptr;
	uint8_t n_cs = 0;
	Pin heater_enable {&PORTA, 3};
		
	PoCController() {

	}
		
	~PoCController() {
			
	}	
		
protected:


	inline void activateTWI() {
		twi_disable.disable();
		led_enable.enable();
	}
	
	inline void deactivateTWI() {
		twi_disable.enable();
		led_enable.disable();
	}
	
	inline void activateSPIComponent(uint8_t n) {
		if(n < n_cs) {		
			if(active_cs)		
				active_cs->enable();		//Disable currently active chip select
			csn_disable[n].disable();		//Enable new chip select
		}
	}
	
	inline void activateSPI() {
		spi_disable.disable();
		led_enable.enable();
	}
	
	inline void deactivateSPI() {
		spi_disable.enable();
		led_enable.disable();
		
		//this might not be necessary
		for(uint8_t i = 0; i < n_cs; i++) {
			csn_disable[i].enable();
		}
	}
	
	virtual const uint8_t getStandardRegisterCount() {
		return 64;
	}
	
	virtual const uint8_t* getStandardRegisterValues() {
		static const uint8_t standard_values[] {
			0, 1, 2, 3, 4, 5, 6, 7, 
			8, 9, 10, 11, 12, 13, 14, 15,
			16, 17, 18, 19, 20, 21, 22, 23,
			24, 25, 26, 27, 28, 29, 30, 31,
			32, 33, 34, 35, 36, 37, 38, 39,
			40, 41, 42, 43, 44, 45, 46, 47,
			48, 49, 50, 51, 52, 53, 54, 55, 
			56, 57, 58, 59, 60, 61, 62, 63
		};
		return standard_values;
	}
	
public:
	static inline PoCController& getInstance() {
		static PoCController instance;
		return instance;
	}
	
	void init(uint8_t address, uint8_t numberOfRegisters = 0, Pin* chipSelects = nullptr, uint8_t numberOfChipSelects = 0, bool copyOldRegisters = false) {
		AtTinyTWISlave::init(address, numberOfRegisters, copyOldRegisters);
		
		csn_disable = chipSelects;
		n_cs = numberOfChipSelects;
		
		for(uint8_t i = 0; i < n_cs; i++) {
			csn_disable[i].enable();			//Disable all chip selects
		}
		
		deactivateSPI();
		deactivateTWI();
	}
	
	virtual void handle() {
		
	}
	
	virtual void interruptHandler() {
		if(stopConditionReceived() && reg_number_received) {			//Enable/Disable TWI/SPI only after a Stop-Condition has been received
			switch(current_register) {
			case 0x00:
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
				activateSPIComponent(current_register);
			}
			
			reg_number_received = false;
		}
		AtTinyTWISlave::interruptHandler();
	}

};

#endif
