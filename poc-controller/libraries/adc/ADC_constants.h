/*
 * ADC_constants.h
 *
 * Created: 12/08/2020 11:03:34
 *  Author: Alex
 */ 

#include <avr/io.h>

#ifndef ADC_CONSTANTS_H_
#define ADC_CONSTANTS_H_

enum class InternalVRef {
	V0_55 = 0x00,
	V1_1 = 0x01,
	V2_5 = 0x02,
	V4_3 = 0x03,
	V1_5 = 0x04
};

enum class Channel {
	AIN0 = 0x00,
	AIN1 = 0x01,
	AIN2 = 0x02,
	AIN3 = 0x03,
	AIN4 = 0x04,
	AIN5 = 0x05,
	AIN6 = 0x06,
	AIN7 = 0x07,
	AIN10 = 0x0A,
	AIN11 = 0x0B,
	DAC = 0x1C,
	INTREF = 0x1D,
	GND = 0x1F
};

enum class RunMode {
	SINGLE = 0x00,
	FREERUN = 0x01
};

enum class Resolution {
	BIT_10 = 0x00,
	BIT_8 = 0x01
};

enum class Prescaler {
	DIV2 = 0x00,
	DIV4 = 0x01,
    DIV8 = 0x02,
    DIV16 = 0x03,
    DIV32 = 0x04,
    DIV64 = 0x05,
    DIV128 = 0x06,
    DIV256 = 0x07,
};

enum class ReferenceVoltage {
	INTERNAL = 0x00,
	VDD = 0x01
};

enum class Accumulation {
	NONE = 0x00,
	ACC2 = 0x01,
	ACC4 = 0x02,
	ACC8 = 0x03,
	ACC16 = 0x04,
	ACC32 = 0x05,
	ACC64 = 0x06
};

enum class ComparatorMode {
    NONE = 0x00,
    Threshold,
    Window
};

enum class ThresholdComparator {
    NOT_THRESHOLD = 0x00,
    BELOW = 0x01,
    ABOVE = 0x02
};

enum class WindowComparator {
    NOT_WINDOW = 0x00,
    INSIDE = 0x03,
    OUTSIDE = 0x04
};


#endif /* ADC_CONSTANTS_H_ */