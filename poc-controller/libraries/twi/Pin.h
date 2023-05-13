/*
* Pin.h
*
* Created: 22.05.2021 19:44:17
* Author: Benjamin
*/


#ifndef __PIN_H__
#define __PIN_H__

#include "avr/io.h"
#include "stdint.h"

class Pin
{
	public:
	PORT_t* const port;
	const uint8_t pin_mask;
	
	Pin(PORT_t* const port, const uint8_t pin, const bool output = true) : port(port), pin_mask(1 << pin) {
		if(output) {
			port->DIRSET = pin_mask;
		}
		else {
			port->DIRCLR = pin_mask;
		}
	}
	
	inline void enable() {
		port->OUTSET = pin_mask;
	}
	
	inline void disable() {
		port->OUTCLR = pin_mask;
	}
	
	inline bool get() {
		return port->IN & (pin_mask);
	}
	
	inline void set(bool enable) {
		port->OUT = (port->OUT & ~(pin_mask)) | (enable ? pin_mask : 0);
	}
	
	inline void toggle() {
		port->OUTTGL = pin_mask;
	}

	inline void setPullUp() {
		switch (this->pin_mask)
		{
			case 0x01 << 0:
			this->port->PIN0CTRL |= 0x01 << 3;	// Enable Pull-Up resistor (see page 152 or chapter 16.5.11  Pin n Control)
			break;
			case 0x01 << 1:
			this->port->PIN1CTRL |= 0x01 << 3;	// Enable Pull-Up resistor (see page 152 or chapter 16.5.11  Pin n Control)
			break;
			case 0x01 << 2:
			this->port->PIN2CTRL |= 0x01 << 3;	// Enable Pull-Up resistor (see page 152 or chapter 16.5.11  Pin n Control)
			break;
			case 0x01 << 3:
			this->port->PIN3CTRL |= 0x01 << 3;	// Enable Pull-Up resistor (see page 152 or chapter 16.5.11  Pin n Control)
			break;
			case 0x01 << 4:
			this->port->PIN4CTRL |= 0x01 << 3;	// Enable Pull-Up resistor (see page 152 or chapter 16.5.11  Pin n Control)
			break;
			case 0x01 << 5:
			this->port->PIN5CTRL |= 0x01 << 3;	// Enable Pull-Up resistor (see page 152 or chapter 16.5.11  Pin n Control)
			break;
			case 0x01 << 6:
			this->port->PIN6CTRL |= 0x01 << 3;	// Enable Pull-Up resistor (see page 152 or chapter 16.5.11  Pin n Control)
			break;
		}
	}
	
	~Pin() {
		
	}
};

#endif
