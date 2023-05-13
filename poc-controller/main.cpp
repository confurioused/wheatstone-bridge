/*
 * PoCControllerV2.cpp
 *
 * Created: 21.05.2021 16:17:02
 * Author : Lucas Nöller
 */ 

#define TWI_ADDRESS 0x30
#define F_CPU 20000000	// ATtiny CPU frequency is 20 MHz (This value is passive and needs to be set to the actual value - it will not change the actual CPU frequency)

#include <avr/io.h>
#include "libraries/twi/PoCController.h"
#include "libraries/clock-controller/ClockController.h"
#include "libraries/adc/ATtiny814_ADC.h"
#include "libraries/onboard-heater/ATtiny814_OnBoardHeater.h"
#include "libraries/onboard-temperature-sensor/ATtiny814_OnBoardTemperatureSensor.h"
#include "libraries/tca/ATtiny814_TCA.h"

PoCController pocC;

int main(void)
{
	pocC.init(TWI_ADDRESS);
	
	// Set clock to 20 MHz
	ATtiny814_ClockController::setClockSource(ATtiny814_ClockController::ClockSource::OSC20M);
	ATtiny814_ClockController::disablePrescaler();
	
    while (true) {
		pocC.handle();
    }
}

ISR(TWI0_TWIS_vect) {
	pocC.interruptHandler();
}

