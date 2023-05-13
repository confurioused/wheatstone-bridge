#include "AtTinyTWIMaster.h"


void AtTinyTWIMaster::init(uint8_t address, uint8_t n_registers, bool copyOldRegisters, bool alternativePinout) {
	twi_saddr = address; //!
	//initTWI()
	//TODO
}

void AtTinyTWIMaster::initTWI(const uint8_t address) {
	//TODO
}

void AtTinyTWIMaster::setMasterStatus(bool masterStatus){
	
	this->masterStatus = masterStatus;
	
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

void AtTinyTWIMaster::writeData(uint8_t address, uint8_t sregister, uint8_t data){
	
	TWI0.MADDR = address << 1 & (~0x01 << 0); //address write
	TWI0.MDATA = sregister; //load slave register in FIFO
	if(!bit_is_set(TWI0.MSTATUS, TWI_ARBLOST_bp)){ //check for bus error
		TWI0.MDATA = data; //load written data in FIFO
	}
}

void AtTinyTWIMaster::write8BitCommand(uint8_t address, uint8_t command){
	TWI0.MADDR = address << 1 & (~0x01 << 0); //address write
	if(!bit_is_set(TWI0.MSTATUS, TWI_ARBLOST_bp)){ //check for bus error
		TWI0.MDATA = command; //load written data in FIFO
	}
	
	TWI0.MCTRLB = 0x3; //stop
}

uint8_t AtTinyTWIMaster::readData(uint8_t address,uint8_t sregister){
	
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

uint16_t AtTinyTWIMaster::readData16Bit(uint8_t address,uint8_t sregister){
	uint16_t data = 0;
	TWI0.MADDR = address << 1 & (~0x01 << 0); //address write
	TWI0.MDATA = sregister; //load slave register in FIFO
	TWI0.MCTRLB = 0x1; //repeated start
	TWI0.MADDR = address<<1 | 0x01<<0; //address read
	
	data = static_cast<uint8_t>(TWI0.MDATA) << 8;
	TWI0.MCTRLB = 0x2; //ACK
	data |=  static_cast<uint8_t>(TWI0.MDATA) << 0;
	TWI0.MCTRLB = 0x7;//NACK+P
	return data;
}

