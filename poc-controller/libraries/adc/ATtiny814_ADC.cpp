/*
 * ATtiny814_ADC.cpp
 *
 * Created: 11/08/2020 12:21:32
 *  Author: Alex
 */

#include "ATtiny814_ADC.h"

ATtiny814_ADC::ATtiny814_ADC(){};

ATtiny814_ADC::ATtiny814_ADC(Channel channel) : ATtiny814_ADC(channel, RunMode::SINGLE, Resolution::BIT_10, Prescaler::DIV2, ReferenceVoltage::VDD, Accumulation::NONE) {}

ATtiny814_ADC::ATtiny814_ADC(Channel channel, RunMode runMode, Resolution resolution, Prescaler prescaler) : ATtiny814_ADC(channel, runMode, resolution, prescaler, ReferenceVoltage::VDD, Accumulation::NONE) {}

ATtiny814_ADC::ATtiny814_ADC(Channel channel, RunMode runMode, Resolution resolution, Prescaler prescaler, ReferenceVoltage reference, Accumulation accumulation) {
	// clear control register
	ADC0.CTRLA = 0x00;
	ADC0.CTRLC = 0x00;

	// set input channel
	ADC0.MUXPOS = CAST(uint8_t, channel);
	
	// set run mode
	ADC0.CTRLA |= (CAST(uint8_t, runMode) << ADC_FREERUN_bp);
	
	// set resolution
	ADC0.CTRLA |= (CAST(uint8_t, resolution) << ADC_RESSEL_bp);
	
	// 10 bit resolution needs at least a prescaler of 16 (results from max. 1.5MHz ADC clock with 16/20MHz main clock)
    if (resolution == Resolution::BIT_10 && CAST(uint8_t, resolution) < CAST(uint8_t, Prescaler::DIV16)) {
        prescaler = Prescaler::DIV16;
    }
    // set prescaler
    ADC0.CTRLC |= (CAST(uint8_t, prescaler) << ADC_PRESC_gp);
	
	// set reference voltage
	ADC0.CTRLC |= (CAST(uint8_t, reference) << ADC_REFSEL_gp);
	
	// set sampling accumulation
	ADC0.CTRLB = CAST(uint8_t, accumulation);
}

void ATtiny814_ADC::enable() {
	setBit(ADC0.CTRLA, ADC_ENABLE_bp);
	
	if (getRunMode() == RunMode::FREERUN) {
		// start first conversation
		setBit(ADC0.COMMAND, ADC_STCONV_bp);
	}
}

void ATtiny814_ADC::disable() {
	clearBit(ADC0.CTRLA, ADC_ENABLE_bp);
}

void ATtiny814_ADC::triggerConversion_async() {
	// no effect in free run mode
	if (getRunMode() == RunMode::SINGLE) {
		// trigger conversation
		setBit(ADC0.COMMAND, ADC_STCONV_bp);
	}
}

uint16_t ATtiny814_ADC::readValue() {
	// return result
	if (this->getResolution() == Resolution::BIT_10)
		return ADC0.RES & 0x03FF;
	else
		return ADC0.RES & 0x00FF;
}

uint16_t ATtiny814_ADC::triggerAndRead() {
	
	// no effect in free run mode
	if (getRunMode() == RunMode::SINGLE) {
		// trigger conversation
    	setBit(ADC0.COMMAND, ADC_STCONV_bp);
		
		// wait till conversion result is ready
		while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
		
		// clear interrupt flag
		setBit(ADC0.INTFLAGS, ADC_RESRDY_bp);
		
		// return result
		return readValue();
	}
	
	return 0x00;
}

double ATtiny814_ADC::toVoltage(uint16_t reg, InternalVRef internal) {
	double v_ref = 0.0;
    switch(internal) {
        case InternalVRef::V0_55:
            v_ref = 0.5;
            break;
        case InternalVRef::V1_1:
            v_ref = 0.5;
            break;
        case InternalVRef::V1_5:
            v_ref = 0.5;
            break;
        case InternalVRef::V2_5:
            v_ref = 0.5;
            break;
        case InternalVRef::V4_3:
            v_ref = 0.5;
            break;
    }
    
    // formula according to 30.3.2.5
    return (reg*v_ref)/1023.0;
}

double ATtiny814_ADC::toVoltage(uint16_t reg, double vdd) {
    // formula according to 30.3.2.5
    return (reg*vdd)/1023.0;
}

void ATtiny814_ADC::enableResultReadyInterrupt() {
	setBit(ADC0.INTCTRL, ADC_RESRDY_bp);
}

void ATtiny814_ADC::enableThresholdComparatorInterrupt(ThresholdComparator mode, uint16_t threshhold) {
	// enable interrupt
	setBit(ADC0.INTCTRL, ADC_WCMP_bp);
	
	if (mode == ThresholdComparator::ABOVE) {
		// set threshold
		ADC0.WINHT = threshhold;
		
	} else {
		// set threshold
		ADC0.WINLT = threshhold;
		
	}
	
	// set mode
	ADC0.CTRLE = CAST(uint8_t, mode);
}

void ATtiny814_ADC::enableWindowComparatorInterrupt(WindowComparator mode, uint16_t upper, uint16_t lower) {
	// enable interrupt
	setBit(ADC0.INTCTRL, ADC_WCMP_bp);
	
	
	// set mode
	ADC0.CTRLE = CAST(uint8_t, mode);
		
	// set window orders
	ADC0.WINHT = upper;
	ADC0.WINLT = lower;
}

void ATtiny814_ADC::clearResultReadyFlag() {
    ADC0.INTFLAGS |= (1 << ADC_RESRDY_bp);
}

void ATtiny814_ADC::clearComparatorFlag() {
    ADC0.INTFLAGS |= (1 << ADC_WCMP_bp);
}

void ATtiny814_ADC::disableResultReadyInterrupt() {
	clearBit(ADC0.INTCTRL, ADC_RESRDY_bp);
}

void ATtiny814_ADC::disableComparatorInterrupt() {
	clearBit(ADC0.INTCTRL, ADC_WCMP_bp);
	
	// set comparator mode to NONE
	ADC0.CTRLE = CAST(uint8_t, ComparatorMode::NONE);
}

/************************************************************************/
/*                          GETTER                                      */
/************************************************************************/

Channel	ATtiny814_ADC::getChannel() {
	return CAST(Channel, (ADC0.MUXPOS & ADC_MUXPOS_gm) >> ADC_MUXPOS_gp);
}

RunMode ATtiny814_ADC::getRunMode() {
	return CAST(RunMode, (ADC0.CTRLA & ADC_FREERUN_bm) >> ADC_FREERUN_bp);
}

Resolution ATtiny814_ADC::getResolution() {
	return CAST(Resolution, (ADC0.CTRLA & ADC_RESSEL_bm) >> ADC_RESSEL_bp);
}
				 
Prescaler ATtiny814_ADC::getPrescaler() {
	return CAST(Prescaler, (ADC0.CTRLC & ADC_PRESC_gm) >> ADC_PRESC_gp);
}
				 
ReferenceVoltage ATtiny814_ADC::getReferenceVoltage() {
	return CAST(ReferenceVoltage, (ADC0.CTRLC & ADC_REFSEL_gm) >> ADC_REFSEL_gp);
}
				 
Accumulation ATtiny814_ADC::getAccumulation() {
	return CAST(Accumulation, (ADC0.CTRLB & ADC_SAMPNUM_gm) >> ADC_SAMPNUM_gp);
}
				 
ComparatorMode ATtiny814_ADC::getComparatorMode() {
	if (ADC0.CTRLE == CAST(uint8_t, ComparatorMode::NONE) || ADC0.CTRLE > CAST(uint8_t, WindowComparator::OUTSIDE))
        return ComparatorMode::NONE;
    else if (ADC0.CTRLE < CAST(uint8_t, WindowComparator::INSIDE))
        return ComparatorMode::Threshold;
    else
        return ComparatorMode::Window;
}


ThresholdComparator ATtiny814_ADC::getThresholdComparator() {
    if (getComparatorMode() == ComparatorMode::Threshold) {
        return CAST(ThresholdComparator, ADC0.CTRLE);
    } else {
        return ThresholdComparator::NOT_THRESHOLD;
    }        
}

WindowComparator ATtiny814_ADC::getWindowComparator() {
    if (getComparatorMode() == ComparatorMode::Window) {
        return CAST(WindowComparator, ADC0.CTRLE);
    } else {
        return WindowComparator::NOT_WINDOW;
    }
}

/************************************************************************/
/*                          SETTER                                      */
/************************************************************************/

void ATtiny814_ADC::setChannel(Channel channel) {
	ADC0.MUXPOS = CAST(uint8_t, channel);
}

void ATtiny814_ADC::setRunMode(RunMode runMode) {
	if (runMode == RunMode::FREERUN) {
		setBit(ADC0.CTRLA, ADC_FREERUN_bp);
	}
	else
	{
		clearBit(ADC0.CTRLA, ADC_FREERUN_bp);
	}
}

void ATtiny814_ADC::setResolution(Resolution resolution) {
	if (resolution == Resolution::BIT_8) {
		setBit(ADC0.CTRLA, ADC_RESSEL_bp);
	}
	else
	{
		clearBit(ADC0.CTRLA, ADC_RESSEL_bp);
	}
}

void ATtiny814_ADC::setPrescaler(Prescaler prescaler) {
	// reset prescaler
	ADC0.CTRLC &= ~ADC_PRESC_gm;
	
	// set new prescaler
	ADC0.CTRLC |= (CAST(uint8_t, prescaler) << ADC_PRESC_gp);
}

void ATtiny814_ADC::setRefereceVoltage(ReferenceVoltage reference) {
	// reset reference voltage
	ADC0.CTRLC &= ~ADC_REFSEL_gm;
	
	// set new reference voltage
	ADC0.CTRLC |= (CAST(uint8_t, reference) << ADC_REFSEL_gp);
}

void ATtiny814_ADC::setAccumulation(Accumulation accumulation) {
	// reset accumulation
	ADC0.CTRLB &= ~ADC_SAMPNUM_gm;
	
	// set new accumulation
	ADC0.CTRLB |= (CAST(uint8_t, accumulation) << ADC_SAMPNUM_gp);
}

ATtiny814_ADC::~ATtiny814_ADC() {
    // disable ADC
    disable();

    // clear interrupts if used
    disableResultReadyInterrupt();
    disableComparatorInterrupt();
}