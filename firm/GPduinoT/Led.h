#ifndef _LED_H_
#define _LED_H_

// LED driver
class Led
{
public:
    // set I2C slave address
    static void setSlaveAddress(int addr);
    
    // constructor
    Led(int ch);
    // begin the LED driver
    void begin();
    // turn off
    void turnOff();
    // turn on
    void turnOn();
    // blink
    void blink(int cycle);
/*
    // set PWM duty
    void write(int duty);
    // set blink cycle and PWM duty
    void blink(int cycle, int duty);
    // call on 10msec cycle timer
    void onTimer();
*/
private:
    // I2C slave address
    static int m_slaveAddress;
    // LEDs state
    static uint16_t m_ledState;
    
    // ch number
    int m_ch;
/*
    // is blinking?
    bool m_isBlinking;
    // blink toggle flag
    bool m_toggleBlink;
    // count for blink
    int m_countBlink;
    // cycle of blink
    int m_cycleBlink;
    // PWM duty (for blink)
    int m_duty;
*/
};

#endif
