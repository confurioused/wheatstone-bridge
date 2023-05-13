/*
 * 500.cpp
 *
 * Created: 25.06.2021 14:51:24
 * Author : Aaron Troll
 */ 

#include "libraries/Messcontroller.h"
#include "components/ADS112C04I.h"


#include <avr/io.h>

#define TWI_ADRESS			 0x00;	//TODO
#define INTERRUPTPIN		PIN7_bm

private bool isMaster = false;

MessController MC;

int main(void)
{
	PORTB_PIN7CTRL = 0x2; //interruptfunction to pin 7?
	MC.init(TWI_ADRESS);
	
	
	while(true) {
		MC.handle();
	}
}

//Register TWI interrupt
ISR(TWI0_TWIS_vect) {
	MC.interruptHandler();
}
ISR(PORTB_PORT){
	MC.commHandler();
	
}
