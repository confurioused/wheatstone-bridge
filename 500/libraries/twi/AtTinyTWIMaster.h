#pragma once
#define SLAVE_BM 0b11100011; //enable all TWI interrupts, enable smart mode, enable TWI client
#define MASTER_BM 0b11010011; // enable read/write interrupt, no timeout, enable smart mode, enable TWI Master, qcen

#include "avr/sfr_defs.h"

#include "avr/io.h"


class AtTinyTWIMaster{
	public:
	AtTinyTWIMaster(){};
	
	bool masterStatus = false;
	
	virtual void init(uint8_t address, uint8_t n_registers = 0, bool copyOldRegisters = false, bool alternativePinout = false);
	
	void initTWI(const uint8_t address);
	
	void setMasterStatus(bool masterStatus);
	
	void writeData(uint8_t address, uint8_t sregister, uint8_t data);
	
	void write8BitCommand(uint8_t address, uint8_t command);
	
	uint8_t readData(uint8_t address,uint8_t sregister);
	
	uint16_t readData16Bit(uint8_t address,uint8_t sregister);
	
	//other functions from TWISlave?
	
	private:
	
	uint8_t twi_saddr;
};