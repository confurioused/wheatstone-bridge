/* 
* ATtiny814_TCA.h
*
* Created: 28.09.2020 14:23:59
* Author: Lucas Nöller
*/

/*
	TODO
		- reset Funktion zusammentragen
		- MIN_PERIOD_ALLOWANCE auf 512 Ticks bei 1 setzen oder so
		- Aktuell wird targetFrequency nur erreicht, wenn die ISR exakt 0 Clockticks benötigt -> zusätzlichen Faktor einbauen
		- setupSplitCompare() testen
		- setFrequencySplit() testen
*/

#ifndef __ATTINY814_TCA_H__
#define __ATTINY814_TCA_H__

// includes
#include <avr/io.h>

// constant for minimal clock pre-scaler
#define MIN_PRE_SCALER 1
// constant for maximum clock pre-scaler
#define MAX_PRE_SCALER 1024
// constant for maximum 16-Bit value
#define MAX_16_BIT_VALUE 65535
// constant for maximum 8-Bit value
#define MAX_8_BIT_VALUE 255
// range of maximum period of TCA register, which is allowed to be used -> very small values will be inaccurate due to interrupt activity and other stuff
#define MIN_PERIOD_ALLOWANCE 0.000f
#define MAX_PERIOD_ALLOWANCE 1.0f

/*
	This will define, whether values are applied to the TCA.SPLIT or TCA.SINGLE registers
*/
enum TCA_MODE
{
	TCA_SINGLE,
	TCA_SPLIT
};

/*
	This will further define the specifications of the timer during SINGLE mode
*/
enum TCA_SINGLE_MODE_SPECS
{
	TCA_SINGLE_NORMAL = (0x00<<0),
	TCA_SINGLE_FREQUENCY = (0x01<<0),
	TCA_SINGLE_SINGLESLOPE = (0x03<<0),
	TCA_SINGLE_DS_TOP = (0x05<<0),
	TCA_SINGLE_DS_BOTH = (0x06<<0),
	TCA_SINGLE_DS_BOTTOM = (0x07<<0)
};

/*
	TCA_HUNF - Only in SPLIT mode -> Interrupt when higher register overflows
	TCA_OVF - In both modes -> Interrupt when TOP (period register value) is reached (works as TCA_LUNF in SPLIT mode)
	TCA_CMP0 - In both modes -> Interrupt when CMP0 value (or LCMP0 in SPLIT mode) is reached
	TCA_CMP1 - In both modes -> Interrupt when CMP1 value (or LCMP1 in SPLIT mode) is reached
	TCA_CMP2 - In both modes -> Interrupt when CMP2 value (or LCMP2 in SPLIT mode) is reached
*/
enum TCA_VECTORS
{
	TCA_HUNF = 0x01 << 1,
	TCA_OVF = 0x01 << 0,
	TCA_CMP0 = 0x01 << 4,
	TCA_CMP1 = 0x01 << 5,
	TCA_CMP2 = 0x01 << 6
};

class ATtiny814_TCA
{
//variables
public:
	// variable for main clock frequency
	unsigned long fCPU = 20000000;
private:
	// variable to store current mode
	TCA_MODE currentMode = TCA_SINGLE;
	TCA_SINGLE_MODE_SPECS currentSingleSpecs;
	// values of possible pre-scaler values
	uint16_t prescalerTCA[8] = {1, 2, 4, 8, 16, 64, 256, 1024};

//functions
public:
	/************************************************************************/
	/*                             FUNCTIONS                                */
	/************************************************************************/
	
    /*
		Standard constructor. Will store frequency of main clock for further use
		Parameter:
			fCPU : unsigned long - Main clock frequency (usually 16/20 MHz)
	*/
	ATtiny814_TCA(unsigned long fCPU);	// advanced constructor
	ATtiny814_TCA();	// standard constructor
	
    /*
		Function will prepare the timer TCA either in SPLIT or SINGLE mode and will enable interrupts according to parameters. Will reset other settings before applying new ones
		Parameters:
			newMode : TCA_MODE - Either TCA_SINGLE for 16-Bit or TCA_SPLIT for two 8-Bit period registers
			[optional] singleMode : TCA_SINGLE_MODE_SPECS - Mode for Timer to define SINGLE mode further
				List of modes:
					TCA_SINGLE_NORMAL - Normal Mode
					TCA_SINGLE_FREQUENCY - Frequency Generation Mode -> up to three PWM signals with 50 % duty cycle
					TCA_SINGLE_SINGLESLOPE - Single Slope PWM
					TCA_SINGLE_DS_TOP - Dual Slope PWM, overflow on TOP
					TCA_SINGLE_DS_BOTH - Dual Slope PWM, overflow on TOP and BOTTOM
					TCA_SINGLE_DS_BOTTOM - Dual Slope PWM, overflow on BOTTOM
	*/
	void setupTCA(TCA_MODE newMode, TCA_SINGLE_MODE_SPECS singleMode = TCA_SINGLE_NORMAL);

	/*
		Function will enable given interrupt vector or disable, depending on enable parameter.
		Parameters:
			interruptVector : TCA_VECTORS - Vector for interrupts
				List of Interrupts:
					TCA_HUNF - Only in SPLIT mode -> Interrupt when higher register overflows
					TCA_OVF - In both modes -> Interrupt when TOP (period register value) is reached (works as TCA_LUNF in SPLIT mode)
					TCA_CMP0 - In both modes -> Interrupt when CMP0 value (or LCMP0 in SPLIT mode) is reached
					TCA_CMP1 - In both modes -> Interrupt when CMP1 value (or LCMP1 in SPLIT mode) is reached
					TCA_CMP2 - In both modes -> Interrupt when CMP2 value (or LCMP2 in SPLIT mode) is reached
			enable : bool - If true (optional value) then this interrupt will vector will be set, if false it will be removed
	*/

	void enableInterrupt(TCA_VECTORS interruptVector, bool enable = true);

	/*
		Changes Period register of given period counter.
	*/
	void setPeriodRegister(uint16_t periodValue, bool isHighRegister = false);
	
    /*
		Set given pre-scaler for TCA in SINGLE mode
		Parameter:
			preScaler : TCA_SINGLE_CLKSEL_enum - Bit mask that represents the new value
	*/
	void setPreScalerSingle(TCA_SINGLE_CLKSEL_enum preScaler);
	
    /*
		Set given pre-scaler for TCA in SPLIT mode
		Parameter:
			preScaler : TCA_SPLIT_CLKSEL_enum - Bit mask that represents the new value
	*/
	void setPreScalerSplit(TCA_SPLIT_CLKSEL_enum preScaler);
	
    /*
		Function will setup a single compare channel of the TCA timer in SINGLE mode
		Parameters:
			compareChannel : uint8_t - ID of compare channel [0, 1, 2]
			compareValue : uint16_t - Compare register value for this channel
			[optional] enableOutput : bool - Defines if compare result is forwarded to corresponding output pin to create PWM signals. Standard is true
		Return:
			uint16_t - Returns set compare channel. Returns zero when TCA is in SPLIT mode
	*/
	uint16_t setupCompareChannel(uint8_t compareChannel, uint16_t compareValue, bool enableOutput = true);

	/*
		Changes compare value of the given channel to resemble the given duty-cycle.
	*/
	void setDutycycle(float dutyCycle, uint8_t channel);
	
    /*
		Calculates clock pre-scaler and period register value to fit targetFrequency the best. Will work on both SINGLE and SPLIT mode
		Parameter:
			targetFrequency : float - Value for target frequency in [Hz]. Minimum and maximum frequency depend on clock-settings
			periodRegister : uint8_t - ID of period register [0, 1]
		Return:
			float - Set frequency of period register in Hz. Will return -1 if TCA is in SPLIT mode or targetFrequency is too low/high
	*/
	float setFrequency(float targetFrequency, uint8_t periodRegister = 0);
	
    /*
		This function will setup the given compareChannel to output a PWM signal with baseFrequency and the given dutyCycle. The function will automatically
		differentiate between SINGLE and SPLIT mode.
		Parameters:
			- baseFrequency : float - Value for target frequency in [Hz]. Minimum and maximum frequency depend on clock-settings
			- compareChannel : uint8_t - ID of compare channel [0, 1, 2, 3, 4, 5]. Channel - Pxn := 0 - PB0, 1 - PB1, 2 - PB2, 3 - PA3, 4 - PA4, 5 - PA5
											Corresponding pairs: 0-3, 1-4, 2-5
			- [optional] dutyCycle : float - Value in percent the duty cycle will have. Standard is 50 %
	*/
	void setPWM(float baseFrequency, uint8_t compareChannel, float dutyCycle = 50.0f);
	
    /*
		Function enables timer to start counting clock ticks
	*/
	void start();
    
    /*
        Function disables timer to stop counting clock ticks
    */
    void stop();
	
	/*
		Check whether the timer is running/was started
	*/
	bool isRunning();
	
    /*
		Function will put TCA timer back to factory setting
	*/
	void reset();

	/************************************************************************/
	/*                         HELPER FUNCTIONS                             */
	/************************************************************************/
	
    /*
		Helper function to determine maximum possible frequency with current clock settings
		Return:
			float - Returns the maximum possible frequency with current clock settings and safety factor
	*/
	float calculateMinFrequency();
	
    /*
		Helper function to determine minimal possible frequency with current clock settings
		Return:
			float - Returns the minimal possible frequency with current clock settings and safety factor
	*/
	float calculateMaxFrequency();
	
    /*
		Will check, if the given frequency is within boundaries.
		Parameter:
			targetFrequency : float - Frequency that is about to be set as new value and needs to be checked
		Return:
			bool - True if in range, false if not
	*/
	bool frequencyWithinRange(float targetFrequency);
	
    /*
		Will check, if the given period is within allowance.
		Parameters:
			periodBuffer : unsigned long - Value that shall be tested
			minPeriod : unsigned long - Minimum allowed period (depends on 8/16-Bit, etc.)
			maxPeriod : unsigned long - Maximum allowed period (depends on 8/16-Bit, etc.)
		Return:
			bool - True if within, false if not within allowance
	*/
	bool periodWithinRange(unsigned long periodBuffer, unsigned long minPeriod, unsigned long maxPeriod);
	
    /*
		Calculates clock pre-scaler and period register value to fit targetFrequency the best
		Parameter:
			targetFrequency : float - Value for target frequency in [Hz]. Minimum and maximum frequency depend on clock-settings
		Return:
			float - Set frequency of period register in Hz. Will return -1 if TCA is in SPLIT mode or targetFrequency is too low/high
	*/
	float setFrequencySingle(float targetFrequency);
	
    /*
		Calculates clock pre-scaler and period register value to fit targetFrequency the best
		Parameters:
			targetFrequency : float - Value for target frequency in [Hz]. Minimum and maximum frequency depend on clock-settings
			periodRegister : uint8_t - Defines which register is used to set the frequency. 0 - Low register, 1 - High register
		Return:
			float - Set frequency of period register in Hz. Will return -1 if TCA is in SINLGE mode or targetFrequency is too low/high
	*/
	float setFrequencySplit(float targetFrequency, uint8_t periodRegister);
	
    /************************************************************************/
	/*                             GETTER                                   */
	/************************************************************************/
	TCA_MODE getCurrentMode();


}; //ATtiny814_TCA

#endif //__ATTINY814_TCA_H__
