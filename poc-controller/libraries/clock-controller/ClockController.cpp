/*
 * ATtiny814_ClockController.cpp
 *
 * Created: 23/08/2020
 *  Author: Alex
 */ 

#include "ClockController.h"

/************************************************************************/
/*                          Main Clock                                  */
/************************************************************************/

unsigned long ATtiny814_ClockController::getF_CPU() {
    
    switch(CLKCTRL.MCLKCTRLA) {
        case 0x0:
            // 20/16MHz clock
            switch (OSC20M::getFrequency()) {
                case OSC20M::Frequency::MHz_20:
                    return 20000000;
                case OSC20M::Frequency::MHz_16:
                    return 16000000;
                default:
                    return 0;
            }
        // 32kHZ ultra low oscillator
        case 0x1:
            return 32000;
        // 32.798kHz external crystal oscillator
        case 0x2:
            return 32768;
        // external crystal oscillator
        case 0x3:
            return 0;
        default:
            return 0;
    }
}

void ATtiny814_ClockController::enablePrescaler() {
    setMaskCCP(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm);
}

void ATtiny814_ClockController::disablePrescaler() {
    clrMaskCCP(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm);
}

void ATtiny814_ClockController::lockMainClockConfig() {
    setMaskCCP(CLKCTRL.MCLKLOCK, CLKCTRL_LOCKEN_bm);
}


void ATtiny814_ClockController::setClockSource(ATtiny814_ClockController::ClockSource source) {
    setMaskCCP(CLKCTRL.MCLKCTRLA, CAST(uint8_t, source));
    
    // wait until stable
    while (isChangingSource());
}

void ATtiny814_ClockController::setPrescaler(ATtiny814_ClockController::MainPrescaler prescaler) {
    // check for valid prescaler
    if (prescaler != ATtiny814_ClockController::MainPrescaler::UNDEFINED) {
        // CCP signature
        CPU_CCP = CCP_IOREG_gc;
        
        setMaskCCP(CLKCTRL.MCLKCTRLB, (CAST(uint8_t, prescaler) << CLKCTRL_PDIV_gp));
    }
}


ATtiny814_ClockController::ClockSource ATtiny814_ClockController::getClockSource() {
    return CAST(ATtiny814_ClockController::ClockSource, CLKCTRL.MCLKCTRLA);
}
     
ATtiny814_ClockController::MainPrescaler ATtiny814_ClockController::getPrescaler() {
    // check for valid values
    if (CLKCTRL.MCLKCTRLB < 0x0D && CLKCTRL.MCLKCTRLB != 0x06 && CLKCTRL.MCLKCTRLB != 0x07) {
        return CAST(ATtiny814_ClockController::MainPrescaler, CLKCTRL.MCLKCTRLB);
    } else {
        return ATtiny814_ClockController::MainPrescaler::UNDEFINED;
    }
}

bool ATtiny814_ClockController::isChangingSource() {
    return CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm;
}


/************************************************************************/
/*                          16/20MHz Osc.                               */
/************************************************************************/

void ATtiny814_ClockController::OSC20M::runInStandby(bool runstdby) {
    if (runstdby) {
        setMaskCCP(CLKCTRL.OSC20MCTRLA, CLKCTRL_RUNSTDBY_bm);
    } else {
        clrMaskCCP(CLKCTRL.OSC20MCTRLA, CLKCTRL_RUNSTDBY_bm);
    }
}

void ATtiny814_ClockController::OSC20M::lockCalibration() {
    setMaskCCP(CLKCTRL.OSC20MCALIBB, (1 << CLKCTRL_LOCK_bm));
}

void ATtiny814_ClockController::OSC20M::setFrequencyCoefficient(uint8_t coefficient) {
    // trim to 6bit value
    coefficient &= 0x3F;
    
    setMaskCCP(CLKCTRL.OSC20MCALIBA, coefficient);
}
    
void ATtiny814_ClockController::OSC20M::setTemperatureCoefficient(uint8_t coefficient) {
    // trim to 4bit value
    coefficient &= 0x0F;
    
    setMaskCCP(CLKCTRL.OSC20MCALIBB, coefficient);
}


bool ATtiny814_ClockController::OSC20M::isLocked() {
    return CLKCTRL.OSC20MCALIBB & CLKCTRL_LOCK_bm;
}

bool ATtiny814_ClockController::OSC20M::isStable() {
    return CLKCTRL.MCLKSTATUS & CLKCTRL_OSC20MS_bm;
}

ATtiny814_ClockController::OSC20M::Frequency ATtiny814_ClockController::OSC20M::getFrequency() {
    switch (FUSE.OSCCFG) {
        case 0x01:
            return Frequency::MHz_16;
        case 0x02:
            return Frequency::MHz_20;
        default:
            return Frequency::BAD_FUSES;
    }
}

uint8_t ATtiny814_ClockController::OSC20M::getFrequencyCoefficient() {
    return CLKCTRL.OSC20MCALIBA & CLKCTRL_CAL20M_gm;
}

uint8_t ATtiny814_ClockController::OSC20M::getTemperatureCoefficient() {
    return CLKCTRL.OSC20MCALIBB & CLKCTRL_TEMPCAL20M_gm;
}


/************************************************************************/
/*                          32kHz ULP Osc.                              */
/************************************************************************/

void ATtiny814_ClockController::OSCULP32K::runInStandby(bool runstdby) {
    if (runstdby) {
        setMaskCCP(CLKCTRL.OSC32KCTRLA, CLKCTRL_RUNSTDBY_bm);
    } else {
        clrMaskCCP(CLKCTRL.OSC32KCTRLA, CLKCTRL_RUNSTDBY_bm);
    }
}


bool ATtiny814_ClockController::OSCULP32K::isStable() {
    return CLKCTRL.MCLKSTATUS & CLKCTRL_OSC32KS_bm;
}


/************************************************************************/
/*                          32.768 Ext. Osc.                            */
/************************************************************************/

void ATtiny814_ClockController::XOSC32K::runInStandby(bool runstdby) {
    if (runstdby) {
        setMaskCCP(CLKCTRL.XOSC32KCTRLA, CLKCTRL_RUNSTDBY_bm);
        } else {
        clrMaskCCP(CLKCTRL.XOSC32KCTRLA, CLKCTRL_RUNSTDBY_bm);
    }
}
    
void ATtiny814_ClockController::XOSC32K::enable() {
    setMaskCCP(CLKCTRL.XOSC32KCTRLA, CLKCTRL_ENABLE_bm);
}
   

void ATtiny814_ClockController::XOSC32K::setStartUpCycles(ATtiny814_ClockController::XOSC32K::CrystalUp cycles) {
    setMaskCCP(CLKCTRL.XOSC32KCTRLA, (CAST(uint8_t, cycles) << CLKCTRL_CSUT_gp));
}
    
void ATtiny814_ClockController::XOSC32K::setSourceType(ATtiny814_ClockController::XOSC32K::SourceType type) {
    setMaskCCP(CLKCTRL.XOSC32KCTRLA, (CAST(uint8_t, type) << CLKCTRL_SEL_bp));
}


bool ATtiny814_ClockController::XOSC32K::isEnabled() {
    return CLKCTRL.XOSC32KCTRLA & CLKCTRL_ENABLE_bm;
}
    
bool ATtiny814_ClockController::XOSC32K::isStable() {
    return CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm;
}
    
ATtiny814_ClockController::XOSC32K::CrystalUp ATtiny814_ClockController::XOSC32K::getStartUpCycles() {
    return CAST(CrystalUp, CLKCTRL.XOSC32KCTRLA & CLKCTRL_CSUT_gm);
}
    
ATtiny814_ClockController::XOSC32K::SourceType ATtiny814_ClockController::XOSC32K::getSourceType() {
    if (CLKCTRL.XOSC32KCTRLA & CLKCTRL_SEL_bm) {
        return SourceType::Clock;
    } else {
        return SourceType::Crystal;
    }
}


/************************************************************************/
/*                          External Cock                               */
/************************************************************************/

bool ATtiny814_ClockController::EXTCLK::isStable() {
    return CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm;
}
