#include "BridgeResistance.h"

#include <iostream>

Module_BridgeResistance::Module_BridgeResistance(short POCC_address, short Mess_adress) : BasicPOCModule("Module Bridge Resistance", POCC_address), MessCcomp("Messcontroller", Mess_adress){
}

void Module_BridgeResistance::init(char singleResistanceInput[12], char halfBridgeInput1[6], char halfBridgeInput2[6]) {
	MessComp.setInputs(singleResistanceInput, halfBridgeInput1,halfBridgeInput2);
}