/*
 * 500.cpp
 *
 * Created: 25.06.2021 14:51:24
 * Author : Aaron Troll
 */ 

#include "libraries/MessController/MessController.h"


#include <avr/io.h>

#define TWI_ADRESS			 0x51
/*
to be set before start
default nur M1
*/



MessController MC;

int main(void)
{
	
	MC.init(TWI_ADRESS);
	

	sei();
	
 	while(true) {

		MC.handle();
 	}
}


ISR(TWI0_TWIS_vect) {
	MC.interruptHandler();
}
ISR(PORTB_PORT_vect){
	
	if( PORTB.INTFLAGS & PIN2_bm)
	{
		PORTB.INTFLAGS &= PIN2_bm;
		MC.commHandler();
	}
}
