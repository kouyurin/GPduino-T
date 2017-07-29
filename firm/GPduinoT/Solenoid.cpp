#include <Arduino.h>
#include "Solenoid.h"

#define PWM_FREQUENCY   1000
#define PWM_RESOLUTION  10
#define PWM_MAX         1023

// constructor
Solenoid::Solenoid(int pinPwm, int chPwm)
{
    m_pinPwm = pinPwm;
    m_chPwm = chPwm;
}

// begin the solenoid driver
void Solenoid::begin()
{
    // initialize PWM
    ledcAttachPin(m_pinPwm, m_chPwm);
    ledcSetup(m_chPwm, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcWrite(m_chPwm, 0);
}

// drive the solenoid
// value: output value (0 to +1023)
void Solenoid::out(int duty)
{
    if(duty<0) duty = 0;
    if(duty>PWM_MAX) duty = PWM_MAX;
    
    ledcWrite(m_chPwm, duty);
}

