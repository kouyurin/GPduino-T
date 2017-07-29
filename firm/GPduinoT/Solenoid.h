#ifndef _SOLENOID_H_
#define _SOLENOID_H_

class Solenoid
{
public:
    // constructor
    Solenoid(int pinPwm, int chPwm);
    // begin the solenoid driver
    void begin();
    // drive the solenoid
    void out(int duty);
    
private:
    int m_pinPwm;   // pin number of PWM
    int m_chPwm;    // channel number of PWM
};

#endif
