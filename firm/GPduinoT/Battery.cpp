#include <Arduino.h>
#include "Battery.h"

// constructor
// pin: pin number of input
// dividerRatio: voltage divider ratio
Battery::Battery(int pin, int dividerRatio)
{
    m_pin = pin;
    m_dividerRatio = dividerRatio;
    isAverageEnable = true;
}

// begin
void Battery::begin()
{
    Average.init();
}

// get the battery voltage (ADC raw value)
int Battery::getAdc()
{
    unsigned short Vbat = analogRead(m_pin);
    
    if(isAverageEnable){
        Vbat = Average.pop(Vbat);
    }
    
    return (int)Vbat;
}

// get the battery voltage [mV]
int Battery::get_mV()
{
    int adc = this->getAdc();
    
    int mV = (adc * m_dividerRatio * 3300) >> 12;
    return mV;
}

// convert ADC raw value to voltage value[mV]
// adc: ADC raw value
int Battery::get_mV(int adc)
{
    int mV = (adc * m_dividerRatio  * 3300) >> 12;
    return mV;
}

// enable the averaging filter
void Battery::averageOn()
{
    isAverageEnable = true;
}

// disable the averaging filter
void Battery::averageOff()
{
    isAverageEnable = false;
}


