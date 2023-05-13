/*
 * PoCControllerV2.cpp
 *
 * Created: 21.05.2021 16:17:02
 * Author : Aaron Troll
 */ 

#define TWI_ADDRESS 0x00 //TODO
#define F_CPU 20000000	// ATtiny CPU frequency is 20 MHz (This value is passive and needs to be set to the actual value - it will not change the actual CPU frequency)
#define TWI_EN (0x01 << 3)	// Standard is PB3
#define TWI_STATUS (0x01 << 5)	// Standard is PA5

#define PORTB_PIN7CTRL = 0x2; //pin 7 Rising edge interrupt
#include <avr/io.h>
#include "libraries/twi/POCController500.h"
#include "libraries/clock-controller/ClockController.h"
#include "libraries/tca/ATtiny814_TCA.h"


POCController500 pocC500; 

int main(void)
{
	pocC500.init(TWI_ADDRESS);
	
	// Set clock to 20 MHz
	ATtiny814_ClockController::setClockSource(ATtiny814_ClockController::ClockSource::OSC20M);
	ATtiny814_ClockController::disablePrescaler();
	
    while (true) {
		pocC500.handle();
    }
}


ISR(TWI0_TWIS_vect) {
	
	pocC500.interruptHandler();
	
	}
