/* 
* AtTinyTWISlave.h
*
* Created: 22.05.2021 19:30:15
* Author: Benjamin
*/


#ifndef __ATTINYTWISLAVE_H__
#define __ATTINYTWISLAVE_H__

#include "stdint.h"
#include "stdlib.h"
#include "avr/interrupt.h"

class AtTinyTWISlave
{
protected:
	uint8_t* reg;	
	uint8_t n_registers;
	
	uint8_t current_register = 0;
	bool reg_number_received = false;

	AtTinyTWISlave() {

	}
	
public:
	
	virtual void init(uint8_t address, uint8_t n_registers = 0, bool copyOldRegisters = false, bool alternativePinout = false) { 
		//new number of registers must be at least getStandardRegisterCount bytes
		uint8_t new_n_registers = n_registers < getStandardRegisterCount() ? getStandardRegisterCount() : n_registers;
		
		if(copyOldRegisters) {
			//copy old register pointer
			uint8_t* old_reg = reg;
			
			//allocate new register memory
			reg = (uint8_t*)malloc(new_n_registers);
					
			//if there already existed register memory
			if(old_reg) {
				//copy old register values to new register memory
				for(uint8_t i = 0; i < (this->n_registers < n_registers ? this->n_registers : n_registers); i++) {
					reg[i] = old_reg[i];
				}
						
				//free old register memory
				free(old_reg);
			}
		}
		else {
			//free old register memory
			free(reg);
			
			//allocate new register memory
			reg = (uint8_t*)malloc(new_n_registers);
			
			//write standard register values
			for(uint8_t i = 0; i < getStandardRegisterCount(); i++) {
				reg[i] = getStandardRegisterValues()[i];
			}
		}
		
		this->n_registers = new_n_registers;

		setAlternativePinout(alternativePinout);
		initTWI(address);		
	}
	
	void initTWI(const uint8_t address) {
		setAddress(address);
		
		//maybe dont activate data interrupt?
		TWI0.SCTRLA = 0b11100011;	//enable all TWI interrupts, enable smart mode, enable twi client
			
		CPUINT.LVL1VEC = TWI0_TWIS_vect_num;
			
		sei();
	}
	
	virtual inline void interruptHandler() {
		if(addressReceived()) {
			TWI0.SCTRLB |= 0x03;		//Wait for Start Condition
		}
		if(byteRequestedByMaster()) {
			TWI0.SDATA = getRegister(current_register);		//load requested register into fifo
			current_register++;								//auto increment
		}
		if(byteSentByMaster()) {
			if(reg_number_received) {
				setRegister(current_register, TWI0.SDATA);	//write selected register with data from fifo
				current_register++;							//auto increment
			}
			else {
				current_register = TWI0.SDATA;				//save the selected register's number
				reg_number_received = true;
			}
		}
		if(stopConditionReceived()) {
			TWI0.SCTRLB |= 0x02;		//Complete Transaction
			reg_number_received = false;
		}
	}
	
	virtual void handle() = 0;

	void setAlternativePinout(bool alternativePosition)
	{
		if (alternativePosition)
		{
			PORTMUX.CTRLB |= 0x01 << 4;
		}
		else
		{
			PORTMUX.CTRLB &= ~(0x01 << 4);
		}
	}


protected:	
	virtual const uint8_t getStandardRegisterCount() {
		return 32;
	}

	virtual const uint8_t* getStandardRegisterValues() {
		static const uint8_t standard_values[] {
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0
		};
		return standard_values;
	}

	inline void setAddress(uint8_t address) {
		TWI0.SADDR = address << 1;
	}
	
	inline uint8_t getAddress() {
		return (TWI0.SADDR & 0b11111110) >> 1; 
	}

	inline uint8_t getRegister(uint8_t n) {
		if(n < n_registers)
			return reg[n];
		return 0;
	}
	
	inline uint16_t getRegisterWord(uint8_t n) {
		return (uint16_t)(getRegister(n)) << 8 | getRegister(n + 1);
	}
	
	inline void setRegister(uint8_t n, uint8_t data) {
		if(n < n_registers)
			reg[n] = data;
	}
	
	inline void setRegisterWord(uint8_t n, uint16_t data) {
		setRegister(n, static_cast<uint8_t>(data >> 8));
		setRegister(n + 1, static_cast<uint8_t>(data & 0x00FF));
	}
	
	~AtTinyTWISlave() {
		
	}
	
	inline bool stopConditionReceived() {
		return (TWI0.SSTATUS & 0b01000001) == 0b01000000;
	}
	
	inline bool addressReceived() {
		return (TWI0.SSTATUS & 0b01000001) == 0b01000001;
	}
	
	inline bool byteRequestedByMaster() {
		return (TWI0.SSTATUS & 0b10000010) == 0b10000010;
	}
	
	inline bool byteSentByMaster() {
		return (TWI0.SSTATUS & 0b10000010) == 0b10000000;
	}

}; 

#endif
