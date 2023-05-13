#pragma once
#import "../infrastructure/BasicPOCModule.h"
#import "util/RepeatedTask.h"
#import <vector> 
#include "../MessComponent/MessComponent.h" //__?

#define POC_TWI_ADR		0x50
#define MESS_TWI_ADR	0x51 
using namespace std;

class Module_BridgeResistance : public BasicPOCModule {
public: 
	
	
	/**
	 * @brief Constructor of this module.
	 * @param address [Optional-but needs to be adjusted on Firmware if changed] TWI address of PoC Controller. Standard is 0x50
	*/
	explicit Module_BridgeResistance(short POCC_address = POC_TWI_ADR, short Mess_adress = MESS_TWI_ADR);

	/**
	* sending Inputconfiguration
	*/
	void init(char singleResistanceInput[12], char halfBridgeInput1[6], char halfBridgeInput2[6]);

private:
	char singleResistanceInput[12] = {1}; //need to be set in project , default M1 input
	char halfBridgeInput1[6] = { 2 };
	char halfBridgeInput2[6] = { 4 };

	MessComponent MessComp;
	RepeatedTask getResistances;
	RepeatedTask getHalfBridges;
	RepeatedTask getTeperatures;
};

