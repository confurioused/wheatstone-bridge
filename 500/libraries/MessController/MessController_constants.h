
const uint16_t THRESHOLD = 0.001; //input offset of OpAmp is ca. 10uV

/**
true if resistance on input M e [1,12] shall be measured
default: all are measured //TODO
TODO: DoubleInputconfig for Wheatstone-half-bridge
*/
const bool SingleInputconfigs[12] = {1,0,0,0,0,0,0,0,0,0,0,0};
	
const uint8_t Poti1Addr  = 0x18;// 0011000;
const uint8_t ADCAddr = 0x40; //1000000;
const uint8_t VBridgeSupply = 19.67;//TODO measure at Testpoint
const uint8_t gainSelect_addr = 0x4E;

const int measurementFieldSize = 200; //TODO

const float Poti1R_AB = 1e3;
const float Poti1R_WB = 0;

