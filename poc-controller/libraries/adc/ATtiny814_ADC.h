/*
 * ATtiny814_ADC.h
 *
 * Created: 11/08/2020 12:21:49
 *  Author: Alex
 */ 

#include <avr/io.h>

#include "ADC_constants.h"

#ifndef ADC_H_
#define ADC_H_

#define setBit(reg, pos)	reg |= (1 << pos)
#define clearBit(reg, pos)	reg &= ~(1 << pos)

#define CAST(type, src)		static_cast<type>(src)	


class ATtiny814_ADC {

public:
	ATtiny814_ADC();
	
	/**
     * Simplest constructor taking only the channel as parameter.
     * The other settings default to RunMode: SINGLE, Resolution: BIT_10,
     *  Prescaler: DIV2, ReferenceVoltage: VDD, Accumulation: NONE
     *
     * @param channel   defines the input source of the ADC
     */
    ATtiny814_ADC(Channel channel);
	
	/**
     * A 10 bit resolution needs at least a prescaler of 16. If the prescaler is to small
     * for the chosen resolution it will be automatically increased to DIV16.
     * The other settings default to ReferenceVoltage: VDD, Accumulation: NONE
     * 
     * @param channel       defines the input source of the ADC
     * @param runMode       determines whether the ADC runs in single conversion or free run 
     * @param resolution    determines the resolution of the result   
     * @param prescaler     determines the prescaler from the main clock to the ADC clock
     */
    ATtiny814_ADC(Channel channel, RunMode runMode, Resolution resolution, Prescaler prescaler);
	
	/**
     * A 10 bit resolution needs at least a prescaler of 16. If the prescaler is to small
     * for the chosen resolution it will be automatically increased to DIV16.
     * 
     * @param channel       defines the input source of the ADC
     * @param runMode       determines whether the ADC runs in single conversion or free run
     * @param resolution    determines the resolution of the result
     * @param prescaler     determines the prescaler from the main clock to the ADC clock
     * @param reference     selects the reference voltage the ADC compares its input to
     * @param accumulation  amount of conversions the ADC calculates the mean of, before storing it in the result register
     */
    ATtiny814_ADC(Channel channel, RunMode runMode, Resolution resolution, Prescaler prescaler, ReferenceVoltage reference, Accumulation accumulation);
	
	void enable();
	
	void disable();
	
	/**
	 * Triggers a conversion but does not wait for the result.
	 * The result can later be processed by an interrupt routine.
	 */
	void triggerConversion_async();
	
	/**
	 * Returns current value of the result registers
	 *
	 * Note that the method does not trigger any conversion and therefore
	 * may be used to handle the result ready interrupt.
	 *
	 * @return	the result of a conversion (register value)
	 */
	uint16_t readValue();
	
	/**
	 * Triggers a conversion and waits for the result.
	 * Note that this function should not be used, when an interrupt is set up,
	 * use triggerConversion_async() instead. 
	 *
	 * @return	the result of the conversion (register value)
	 */
	uint16_t triggerAndRead();
	
	/**
	 * Converts the 16bit result of a conversion to the corresponding voltage.
     * This method may be used if the selected reference voltage was an internal reference voltage.
     *
	 * @param reg	    16bit register readout of a previous conversion
     * @param internal  internal reference voltage used for the conversion
	 * @return		    voltage at the pin in volts represented as double
	 */
	double toVoltage(uint16_t reg, InternalVRef internal);
	
    /**
	 * Converts the 16bit result of a conversion to the corresponding voltage.
	 * This method may be used if the selected reference voltage was the supply voltage.
     *
	 * @param reg	16bit register readout of a previous conversion
     * @param vdd   supply voltage during the conversion in volts
	 * @return		voltage at the pin in volts represented as double
	 */
	double toVoltage(uint16_t reg, double vdd);
    
	/**
	 * Sets up the result ready interrupt.
	 * 
	 * Note that the method neither calls sei() nor specifies content for the 
	 * vector ADC0_RESRDY (number: 17). This has to be done by the user.
	 *
	 * Note that the corresponding interrupt flag has to be cleared during the interrupt handle.
	 * One may use the method clearResultReadyFlag for this.
	 */
	void enableResultReadyInterrupt();
	
	/**
	 * Sets up the window compare interrupt for the given parameters.
	 * 
	 * Note that the method neither calls sei() nor specifies an ISR for the 
	 * vector ADC0_WCOMP (number: 18). This has to be done by the user.
	 *
	 * Note that the corresponding interrupt flag has to be cleared during the interrupt handle.
	 * One may use the method clearComparatorFlag for this.
	 * 
	 * @param mode			mode the comparator shall use
	 * @param threshold		threshold values for the comparison
	 */
	void enableThresholdComparatorInterrupt(ThresholdComparator mode, uint16_t threshhold);
	
	/**
	 * Sets up the window compare interrupt for the given parameters.
	 * 
	 * Note that the method neither calls sei() nor specifies an ISR for the 
	 * vector ADC0_WCOMP (number: 18). This has to be done by the user.
	 * 
	 * Note that the corresponding interrupt flag has to be cleared during the interrupt handle.
	 * One may use the method clearComparatorFlag for this.
	 * 
	 * @param mode			mode the comparator shall use
	 * @param threshold		threshold values for the comparison
	 */
	void enableWindowComparatorInterrupt(WindowComparator mode, uint16_t upper, uint16_t lower);
	
    /**
	 * Clears the result ready interrupt flag.
     */
    void clearResultReadyFlag();
    
    /**
	 * Clears the comparator match interrupt flag.
     */
    void clearComparatorFlag();
    
	/**
	 * Disables the result ready interrupt.
	 * 
	 * Note that the method does not call cli(). This has to be done by the user if desired.
	 */
	void disableResultReadyInterrupt();
	
	/**
	 * Disables the window compare interrupt.
	 * 
	 * Note that the method does not call cli(). This has to be done by the user if desired.
	 */
	void disableComparatorInterrupt();
	
    /************************************************************************/
    /*                          GETTER                                      */
    /************************************************************************/

	Channel getChannel();
	
	RunMode getRunMode();
	
	Resolution getResolution();
	
	Prescaler getPrescaler();
	
	ReferenceVoltage getReferenceVoltage();
	
	Accumulation getAccumulation();
	
	ComparatorMode getComparatorMode();
    
    ThresholdComparator getThresholdComparator();
    
    WindowComparator getWindowComparator();

    /************************************************************************/
    /*                          SETTER                                      */
    /************************************************************************/
	
	void setChannel(Channel channel);
	
	void setRunMode(RunMode runMode);
	
	void setResolution(Resolution resolution);
	
	void setPrescaler(Prescaler prescaler);
	
	void setRefereceVoltage(ReferenceVoltage reference);
	
	void setAccumulation(Accumulation accumulation);
	
	~ATtiny814_ADC();
};



#endif /* ADC_H_ */