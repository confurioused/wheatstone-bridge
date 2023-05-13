/*
 * PoCControllerV2.cpp
 *
 * Created: 21.05.2021 16:17:02
 * Author : Benjamin
 */ 

#include <avr/io.h>
#include "PoCController.h"

PoCController pocC;

int main(void)
{
	//PoCController::getInstance().init(0x43);
	
	pocC.init(0x43);
	
    while (true) {
		//PoCController::getInstance().handle();
		pocC.handle();
    }
}

ISR(TWI0_TWIS_vect) {
	//PoCController::getInstance().interruptHandler();
	pocC.interruptHandler();
}

