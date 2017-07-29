#ifndef    _BATTERY_H_
#define    _BATTERY_H_

#include "MovingAverage.h"

// battery measurement class
class Battery
{
public:
    // constructor
    Battery(int pin, int dividerRatio);
    // begin
    void begin();
    // get the battery voltage (ADC raw value)
    int getAdc();
    // get the battery voltage [mV]
    int get_mV();
    // convert ADC raw value to voltage value[mV]
    int get_mV(int adc);
    // enable the averaging filter
    void averageOn();
    // disable the averaging filter
    void averageOff();
private:
    // pin number of input
    int m_pin;
    // voltage divider ratio
    int m_dividerRatio;
    // averaging filter
    MovingAverage Average;
    // is averaging enable?
    bool isAverageEnable;
};

#endif
