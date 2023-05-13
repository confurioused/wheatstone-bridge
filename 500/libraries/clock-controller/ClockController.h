/*
 * ATtiny814_ClockController.h
 *
 * Created: 23/08/2020
 *  Author: Alex
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef CLOCKCONTROLLER_H_
#define CLOCKCONTROLLER_H_

#define CAST(type, src)         static_cast<type>(src)
#define NotNull(a)              (static_cast<uint8_t>(a) != 0x00)

// the following both defines ensure, that the value is already written in time (while CCP is written)
#define setMaskCCP(reg, mask)   uint8_t val  = reg | mask;\
                                CPU_CCP = CCP_IOREG_gc;\
                                reg = val;

#define clrMaskCCP(reg, mask)   uint8_t val  = reg & ~mask;\
                                CPU_CCP = CCP_IOREG_gc;\
                                reg = val;


/************************************************************************/
/*                          Clock Controller                            */
/************************************************************************/

class ATtiny814_ClockController {
private:
    // pure virtual function to keep the class abstract
    virtual void stayAbstract() final = 0;
    
public:
    
    /************************************************************************/
    /*                          Main Clock                                  */
    /************************************************************************/
    
    enum class ClockSource {
        OSC20M = 0x00,
        OSCULP32K = 0x01,
        XOSC32K = 0x02,
        EXTCLK = 0x03
    };

    enum class MainPrescaler {
        DIV2  = 0x0,
        DIV4  = 0x1,
        DIV8  = 0x2,
        DIV16 = 0x3,
        DIV32 = 0x4,
        DIV64 = 0x5,
        DIV6  = 0x8,
        DIV10 = 0x9,
        DIV12 = 0xA,
        DIV24 = 0xB,
        DIV48 = 0xC,
        UNDEFINED
    };
    
    static bool checkF_CPU();
    
    static unsigned long getF_CPU();
    
    static void enablePrescaler();
    
    static void disablePrescaler();
    
    // only unlockable via hardware reset
    // contains prescaler settings and clock source select
    static void lockMainClockConfig();
    
    
    static void setClockSource(ClockSource source);
    
    static void setPrescaler(MainPrescaler prescaler);
    
    
    static ClockSource getClockSource();
    
    static MainPrescaler getPrescaler();
    
    static bool isChangingSource();
    
    /************************************************************************/
    /*                          16/20MHz Osc.                               */
    /************************************************************************/
    
    class OSC20M
    {
    private:
        // pure virtual function to keep the class abstract
        virtual void stayAbstract() final = 0;
    public:
        enum class Frequency {
            MHz_16 = 0x01,
            MHz_20 = 0x02,
            BAD_FUSES
        };
        
        
        static void runInStandby(bool runstdby);
        
        static void lockCalibration();
        
        
        static void setFrequencyCoefficient(uint8_t coefficient);
        
        static void setTemperatureCoefficient(uint8_t coefficient);
        
        
        static bool isLocked();
        
        static bool isStable();
        
        static Frequency getFrequency();
        
        static uint8_t getFrequencyCoefficient();
        
        static uint8_t getTemperatureCoefficient();
    };
    
    /************************************************************************/
    /*                          32kHz ULP Osc.                              */
    /************************************************************************/
    
    class OSCULP32K
    {
    private:
        // pure virtual function to keep the class abstract
        virtual void stayAbstract() final = 0;
        
    public:
        static void runInStandby(bool runstdby);
        
        
        static bool isStable();
    };
    
    /************************************************************************/
    /*                          32.768 Ext. Osc.                            */
    /************************************************************************/
    
    class XOSC32K
    {
        private:
            // pure virtual function to keep the class abstract
            virtual void stayAbstract() final = 0;
        
        public:
            enum class CrystalUp {
                Cycles_1k  = 0x00,
                Cycles_16k = 0x01,
                Cycles_32k = 0x02,
                Cycles_62k = 0x03
            };
        
            enum class SourceType {
                Crystal = 0x00, // ext. crystal connected to TOSC1 TOSC2
                Clock = 0x01    // ext. clock connected to TOSC1
            };
        
        
            static void runInStandby(bool runstdby);
        
            // TOSC* will be configured as input and SEL and CSUT become read-only
            static void enable();
        
        
            static void setStartUpCycles(CrystalUp cycles); // write protected when oscillator on (ENABLE = 1)
        
            static void setSourceType(SourceType type);     // write protected when oscillator on (ENABLE = 1)
        
        
            static bool isEnabled();
            
            static bool isStable();
        
            static CrystalUp getStartUpCycles();
        
            static SourceType getSourceType();
    };
    
    /************************************************************************/
    /*                          External Cock                               */
    /************************************************************************/
    
    // PA3 will e automatically configured for EXTCLK when requested. Start-up time is 2 cycles.
    class EXTCLK
    {
        private:
            // pure virtual function to keep the class abstract
            virtual void stayAbstract() final = 0;
          
        public:
            static bool isStable();
    };
};

#endif /* CLOCKCONTROLLER_H_ */