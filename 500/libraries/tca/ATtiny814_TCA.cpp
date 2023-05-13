/* 
* ATtiny814_TCA.cpp
*
* Created: 28.09.2020 14:23:59
* Author: Lucas Nöller
*/

#include "ATtiny814_TCA.h"

 ATtiny814_TCA::ATtiny814_TCA(unsigned long fCPU)
 {
	this->fCPU = fCPU;
 }

 ATtiny814_TCA::ATtiny814_TCA()
 {

 }

void ATtiny814_TCA::setupTCA(TCA_MODE newMode, TCA_SINGLE_MODE_SPECS singleMode /*= TCA_SINGLE_NORMAL*/)
{
	// before attempting change major settings, put everything back to normal
	this->reset();

	// set mode and submode
	if (newMode == TCA_SINGLE)
	{
		// Put TCA to given sub-mode
		TCA0.SINGLE.CTRLB &= ~(0x07);	// clear bits until reset() works
		TCA0.SINGLE.CTRLB |= singleMode;
		this->currentSingleSpecs = singleMode;
		// enable interrupt for period overflow
		TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
	}
	else
	{		
		// enable split mode for timer
		TCA0.SPLIT.CTRLD &= ~(0x01);	// clear bit until reset() works
		TCA0.SPLIT.CTRLD |= TCA_SPLIT_SPLITM_bm;
		// enable interrupt for period overflow
		TCA0.SPLIT.INTCTRL |= TCA_SPLIT_LUNF_bm | TCA_SPLIT_HUNF_bm | TCA_SINGLE_CMP0_bm | TCA_SINGLE_CMP1_bm | TCA_SINGLE_CMP2_bm; // bitmap for single and split as similar
	}

	this->currentMode = newMode;
}

void ATtiny814_TCA::enableInterrupt(TCA_VECTORS interruptVector, bool enable /*= true*/)
{
	if (this->getCurrentMode() == TCA_SINGLE)
	{
		TCA0.SINGLE.INTCTRL = (TCA0.SINGLE.INTCTRL & ~(interruptVector)) | (enable ? interruptVector : 0);
	} 
	else
	{
		TCA0.SPLIT.INTCTRL = (TCA0.SPLIT.INTCTRL & ~(interruptVector)) | (enable ? interruptVector : 0);
	}
}

void ATtiny814_TCA::setPeriodRegister(uint16_t periodValue, bool isHighRegister /*= false*/)
{
	if(this->currentMode == TCA_SINGLE)
	{
		TCA0.SINGLE.PER = periodValue;
	}
	else
	{
		if (isHighRegister)
		{
			TCA0.SPLIT.HPER = static_cast<uint8_t>(periodValue);
		}
		else
		{
			TCA0.SPLIT.LPER = static_cast<uint8_t>(periodValue);
		}
	}
}

void ATtiny814_TCA::setDutycycle(float dutyCycle, uint8_t channel)
{
	uint16_t newCompareValue;
	if (this->currentMode == TCA_SINGLE)
	{
		newCompareValue = TCA0.SINGLE.PER * dutyCycle;
	} 
	else
	{
		if (channel < 3)
		{
			newCompareValue = TCA0.SPLIT.LPER * dutyCycle;
		} 
		else
		{
			newCompareValue = TCA0.SPLIT.HPER * dutyCycle;
		}
	}
	this->setupCompareChannel(channel, newCompareValue);
}

void ATtiny814_TCA::setPWM(float baseFrequency, uint8_t compareChannel, float dutyCycle /*= 50.0f*/)
{
	if (compareChannel < 3)
	{
		this->setFrequency(baseFrequency, 0);
	} 
	else
	{
		this->setFrequency(baseFrequency, 1);
	}
	volatile float factor = dutyCycle / 100.0f;

	if (this->currentMode == TCA_SINGLE)
	{
		if (this->currentSingleSpecs != TCA_SINGLE_FREQUENCY)
		{
			this->setupCompareChannel(compareChannel, TCA0.SINGLE.PER * factor);
		}
	}
	else
	{
		if(compareChannel < 3)
		{
			this->setupCompareChannel(compareChannel, TCA0.SPLIT.LPER * factor);
		}
		else
		{
			this->setupCompareChannel(compareChannel, TCA0.SPLIT.HPER * factor);
		}
	}
}

uint16_t ATtiny814_TCA::setupCompareChannel(uint8_t compareChannel, uint16_t compareValue, bool enableOutput /*= true*/)
{
	if (this->currentMode == TCA_SINGLE)
	{
		switch (compareChannel)
		{
			case 0:	// compare channel 0
			if (enableOutput)
			{
				PORTB.DIR |= PIN0_bm;	// enable pin 9 (PB0) as output for this channel
				TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;
			}
			else
			{
				TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm);
			}
			TCA0.SINGLE.CMP0 = compareValue;	// set compare value
			break;

			case 1:	// compare channel 1
			if (enableOutput)
			{
				PORTB.DIR |= PIN1_bm;	// enable pin 8 (PB1) as output for this channel
				TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP1EN_bm;
			}
			else
			{
				TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm);
			}
			TCA0.SINGLE.CMP1 = compareValue;	// set compare value
			break;

			case 2:	// compare channel 2
			if (enableOutput)
			{
				PORTB.DIR |= PIN2_bm;	// enable pin 7 (PB2) as output for this channel
				TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP2EN_bm;
			}
			else
			{
				TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP2EN_bm);
			}
			TCA0.SINGLE.CMP2 = compareValue;	// set compare value
			break;
		}		
		// enable compare channel and set waveform generation mode
		TCA0.SINGLE.CTRLB |= this->currentMode;
		return compareChannel;
	}
	else
	{		
		switch (compareChannel)
		{
			case 0: // Pin PB0
			if (enableOutput)
			{
				PORTB.DIR |= PIN0_bm;
				TCA0.SPLIT.CTRLB |= TCA_SPLIT_LCMP0EN_bm;
			}
			else
			{
				TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_LCMP0EN_bm);
			}
			TCA0.SPLIT.LCMP0 = compareValue;
			break;

			case 1: // Pin PB1
			if (enableOutput)
			{
				PORTB.DIR |= PIN1_bm;
				TCA0.SPLIT.CTRLB |= TCA_SPLIT_LCMP1EN_bm;
			}
			else
			{
				TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_LCMP1EN_bm);
			}
			TCA0.SPLIT.LCMP1 = compareValue;
			break;
			case 2: // Pin PB2
			if (enableOutput)
			{
				PORTB.DIR |= PIN2_bm;
				TCA0.SPLIT.CTRLB |= TCA_SPLIT_LCMP2EN_bm;
			}
			else
			{
				TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_LCMP2EN_bm);
			}
			TCA0.SPLIT.LCMP2 = compareValue;
			break;
			case 3: // Pin PA3
			if (enableOutput)
			{
				PORTA.DIR |= PIN3_bm;
				TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP0EN_bm;
			}
			else
			{
				TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_HCMP0EN_bm);
			}
			TCA0.SPLIT.HCMP0 = compareValue;
			break;
			case 4: // Pin PA4
			if (enableOutput)
			{
				PORTA.DIR |= PIN4_bm;
				TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP1EN_bm;
			}
			else
			{
				PORTA.DIR &= ~(PIN4_bm);
			}
			TCA0.SPLIT.HCMP1 = compareValue;
			break;
			case 5: // Pin PA5
			if (enableOutput)
			{
				PORTA.DIR |= PIN5_bm;
				TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP2EN_bm;
			}
			else
			{
				TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_HCMP2EN_bm);
			}
			TCA0.SPLIT.HCMP2 = compareValue;
			break;
		}
		return compareChannel;
	}
	return 0;
}

void ATtiny814_TCA::start()
{
	if (this->currentMode == TCA_SINGLE)
	{
		// start TCA in SINGLE mode
		TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
	}
	else
	{
		// start TCA in SPLIT mode
		TCA0.SPLIT.CTRLA |= TCA_SPLIT_ENABLE_bm;
	}
}

bool ATtiny814_TCA::isRunning() {
	if (this->currentMode == TCA_SINGLE)
	{
		// TCA in SINGLE mode
		return TCA0.SINGLE.CTRLA | TCA_SINGLE_ENABLE_bm;
	}
	else
	{
		// TCA in SPLIT mode
		return TCA0.SPLIT.CTRLA | TCA_SPLIT_ENABLE_bm;
	}
}

void ATtiny814_TCA::stop() {
    if (this->currentMode == TCA_SINGLE)
    {
        // start TCA in SINGLE mode
        TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
    }
    else
    {
        // start TCA in SPLIT mode
        TCA0.SPLIT.CTRLA &= ~TCA_SPLIT_ENABLE_bm;
    }
}

void ATtiny814_TCA::reset()
{
	// Not working, yet
}

float ATtiny814_TCA::setFrequency(float targetFrequency, uint8_t periodRegister /*= 0*/)
{
	if (this->frequencyWithinRange(targetFrequency))
	{
		if(this->getCurrentMode() == TCA_SINGLE)
		{
			return (this->setFrequencySingle(targetFrequency));
		}
		else
		{
			return this->setFrequencySplit(targetFrequency, periodRegister);
		}
	}
	return -1;
}

float ATtiny814_TCA::setFrequencySingle(float targetFrequency)
{
	unsigned long periodBuffer;
	uint8_t i;
	for (i = 0; i < sizeof(this->prescalerTCA)/sizeof(this->prescalerTCA[0]); i++)
	{
		periodBuffer = (this->fCPU) / (targetFrequency * this->prescalerTCA[i]) * 1.011 + 0;	// frequency correction with 1.011
		if ((this->currentSingleSpecs == TCA_SINGLE_NORMAL) || (this->currentSingleSpecs == TCA_SINGLE_SINGLESLOPE))
		{
			if (this->periodWithinRange(periodBuffer, MIN_PERIOD_ALLOWANCE * MAX_16_BIT_VALUE, MAX_PERIOD_ALLOWANCE * MAX_16_BIT_VALUE))
			{
				break;
			}
		}
		else
		{
			if (this->periodWithinRange(periodBuffer, MIN_PERIOD_ALLOWANCE * MAX_16_BIT_VALUE, MAX_PERIOD_ALLOWANCE * MAX_16_BIT_VALUE * 2.0))
			{
				break;
			}
		}
	}
	TCA0.SINGLE.CTRLA &= ~(0x07 << 1);	// clear clock select bits
	TCA0.SINGLE.CTRLA |= i << 1;	// offset of CTRLA register for clock select values
	// depending on mode, the frequency will have to be set differently
	switch (this->currentSingleSpecs)	// current settings of single mode
	{
		case TCA_SINGLE_NORMAL:	// normal mode
			TCA0.SINGLE.PER = periodBuffer;
		break;
		case TCA_SINGLE_SINGLESLOPE:	// single slope mode
			TCA0.SINGLE.PER = periodBuffer;
		break;
		case TCA_SINGLE_FREQUENCY:	// frequency mode -> period values get saved in compare registers here. to individually set those registers use setupSingleCompare()
			TCA0.SINGLE.CMP0 = periodBuffer / 2.0;
			TCA0.SINGLE.CMP1 = periodBuffer / 2.0;
			TCA0.SINGLE.CMP2 = periodBuffer / 2.0;
			return (this->fCPU / (periodBuffer * this->prescalerTCA[i] * 2.0));
		break;
		default:	// dual slope modes
			TCA0.SINGLE.PER = periodBuffer / 2.0;
			return (this->fCPU / (periodBuffer * this->prescalerTCA[i] * 2.0));
		break;
	}

	return (this->fCPU / (periodBuffer * this->prescalerTCA[i]));
}

float ATtiny814_TCA::setFrequencySplit(float targetFrequency, uint8_t periodRegister)
{
	if (this->frequencyWithinRange(targetFrequency) && this->currentMode == TCA_SPLIT)
	{
		uint16_t periodBuffer;
		unsigned int i = 0;
		for (i = 0; i < sizeof(this->prescalerTCA)/sizeof(this->prescalerTCA[0]); i++)
		{
			periodBuffer = fCPU / (targetFrequency * this->prescalerTCA[i]);
			if (this->periodWithinRange(periodBuffer, MIN_PERIOD_ALLOWANCE * MAX_8_BIT_VALUE, MAX_PERIOD_ALLOWANCE * MAX_8_BIT_VALUE))
			{
				break;
			}
		}
		// Set period of TCA SPLIT and pre-scaler
		TCA0.SPLIT.CTRLA &= ~(0x07 << 1);	// clear clock select bits
		TCA0.SPLIT.CTRLA = i << 1;	// offset of CTRLA register for clock select values
		switch (periodRegister)
		{
			case 0:	// Low register
			TCA0.SPLIT.LPER = periodBuffer;
			break;
			default: // High register
			TCA0.SPLIT.HPER = periodBuffer;
			break;
		}

		return (fCPU / (periodBuffer * this->prescalerTCA[i]));
	}
	else
	{
		return -1.0;
	}
}

float ATtiny814_TCA::calculateMinFrequency()
{
	float calculatedFrequency;
	if (this->currentMode == TCA_SINGLE)
	{
		if ((this->currentSingleSpecs == TCA_SINGLE_NORMAL) || (this->currentSingleSpecs == TCA_SINGLE_SINGLESLOPE))
		{
			calculatedFrequency = this->fCPU / (MAX_PRE_SCALER * MAX_PERIOD_ALLOWANCE * MAX_16_BIT_VALUE);
		} 
		else
		{
			calculatedFrequency = this->fCPU / (MAX_PRE_SCALER * MAX_PERIOD_ALLOWANCE * MAX_16_BIT_VALUE * 2.0);
		}
	} 
	else
	{
		calculatedFrequency = this->fCPU / (MAX_PRE_SCALER * MAX_PERIOD_ALLOWANCE * MAX_8_BIT_VALUE);
	}
	return calculatedFrequency;
}

float ATtiny814_TCA::calculateMaxFrequency()
{
	float calculatedFrequency;
	if (this->currentMode == TCA_SINGLE)
	{
		calculatedFrequency = this->fCPU / (MIN_PRE_SCALER * MIN_PERIOD_ALLOWANCE * MAX_16_BIT_VALUE + 1);
	} 
	else
	{
		calculatedFrequency = this->fCPU / (MIN_PRE_SCALER * MIN_PERIOD_ALLOWANCE * MAX_8_BIT_VALUE + 1);
	}
	return calculatedFrequency;
}

bool ATtiny814_TCA::frequencyWithinRange(float targetFrequency)
{
	if (targetFrequency <= this->calculateMaxFrequency() && targetFrequency >= this->calculateMinFrequency())
	{
		return true;
	} 
	else
	{
		return false;
	}
}

bool ATtiny814_TCA::periodWithinRange(unsigned long periodBuffer, unsigned long minPeriod, unsigned long maxPeriod)
{
	if (periodBuffer <= maxPeriod && periodBuffer >= minPeriod)
	{
		return true;
	} 
	else
	{
		return false;
	}
}

void ATtiny814_TCA::setPreScalerSingle(TCA_SINGLE_CLKSEL_enum preScaler)
{
	TCA0.SINGLE.CTRLA |= preScaler;
}

void ATtiny814_TCA::setPreScalerSplit(TCA_SPLIT_CLKSEL_enum preScaler)
{
	TCA0.SPLIT.CTRLA |= preScaler;
}

TCA_MODE ATtiny814_TCA::getCurrentMode()
{
	return this->currentMode;
}
