#include "stdint.h"


const uint8_t COMMAND_WREG = 0x40;
const uint8_t COMMAND_RESET = 0x06;
const uint8_t COMMAND_RDATA = 0x10;
const uint8_t COMMAND_START = 0x08;
const uint8_t WREG_bm = 0x0C;
const uint8_t MUX_bm = 0xF0;
const uint8_t MUX3_bm = 0x0B << 4;
const uint8_t INPUTPORT = MUX3_bm;

const float ADC_RESOLUTION = 2^16;
const uint8_t Vref = 2.048;

enum GAIN
{
	GAIN_1 = 0x0,
	GAIN_2 = 0x1,
	GAIN_4 = 0x2,
	GAIN_8 = 0x3,
	GAIN_16 = 0x4,
	GAIN_32 = 0x5,
	GAIN_64 = 0x6,
	GAIN_128 = 0x7
};
