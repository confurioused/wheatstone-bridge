#pragma once
#define SLAVE_BM 0b11100011; //enable all TWI interrupts, enable smart mode, enable TWI client
#define MASTER_BM 0b11010011; // enable read/write interrupt, no timeout, enable smart mode, enable TWI Master, qcen

#include "avr/sfr_defs.h"

#include "avr/io.h"


class AtTinyTWIMaster{
	private:
	
	uint8_t twi_saddr;
	public:
	
	bool masterStatus = false;
	
	void setMaster(bool masterStatus){
		
		this->masterStatus= masterStatus;
		
		if(this->masterStatus){
			TWI0.MCTRLA = MASTER_BM;
			TWI0.SCTRLA &= ~TWI_ENABLE_bm; //disable slave mode
			
		}
		else{
			TWI0.MCTRLA &= ~TWI_ENABLE_bm; //disable master mode
			TWI0.SADDR = twi_saddr << 1; //write slave address
			TWI0.SCTRLA = SLAVE_BM;
		}
	}
	
	void writeData(uint8_t address, uint8_t sregister, uint8_t data){
	
		TWI0.MADDR = address << 1 & (~0x01 << 0); //address write
		TWI0.MDATA = sregister; //load slave register in FIFO
		if(!bit_is_set(TWI0.MSTATUS, TWI_ARBLOST_bp)){ //check for bus error
			TWI0.MDATA = data; //load written data in FIFO
		}
	}
	
	
	uint8_t readData(uint8_t address,uint8_t sregister){
		
		//adress write
		//register
		//repeated startcondition
		//adress read
		TWI0.MADDR = address << 1 & (~0x01 << 0); //address write
		TWI0.MDATA = sregister; //load slave register in FIFO
		TWI0.MCTRLB = 0x1;
		TWI0.MADDR = address<<1 | 0x01<<0; //address read
		
		TWI0.MCTRLB = 0x3;
		return static_cast<uint8_t>(TWI0.MDATA);
		
		
	}
	
	
	
	
	virtual void init(uint8_t address, uint8_t n_registers = 0, bool copyOldRegisters = false, bool alternativePinout = false) {
		twi_saddr = twi_saddr; //!
		//TODO
	}
	void initTWI(const uint8_t address) {
		//TODO
	}
	
	
	//other functions from TWISlave?
};