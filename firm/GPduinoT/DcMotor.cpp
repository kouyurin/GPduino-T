#include <Arduino.h>

#include "DcMotor.h"

#define PWM_FREQUENCY   1000
#define PWM_RESOLUTION  10
#define PWM_MAX         1023

// constructor
// pinIn1: pin number of IN1
// pinIn2: pin number of IN2
// pinPwm: pin number of PWM
// chPwm: channel number of PWM
DcMotor::DcMotor(int pinIn1, int pinIn2, int pinPwm, int chPwm)
{
    m_pinIn1 = pinIn1;
    m_pinIn2 = pinIn2;
    m_pinPwm = pinPwm;
    m_chPwm = chPwm;
}

// begin the DC motor driver
void DcMotor::begin()
{
    // initialize PWM
    ledcAttachPin(m_pinPwm, m_chPwm);
    ledcSetup(m_chPwm, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcWrite(m_chPwm, 0);
    
    // initialize GPIO
    digitalWrite(m_pinIn1, LOW);
    digitalWrite(m_pinIn2, LOW);
    pinMode(m_pinIn1, OUTPUT);
    pinMode(m_pinIn2, OUTPUT);
}

// drive the motor
// value: output value (-1023 to +1023)
void DcMotor::out(int value)
{
    int pwm = abs(value);
    if(pwm<0) pwm = 0;
    if(pwm>PWM_MAX) pwm = PWM_MAX;
    
    int in1,in2;
    if(value > 0){
        in1 = LOW;
        in2 = HIGH;
    }else if(value < 0){
        in1 = HIGH;
        in2 = LOW;
    }else{
        in1 = LOW;
        in2 = LOW;
        pwm = 0;
    }

    digitalWrite(m_pinIn1, in1);
    digitalWrite(m_pinIn2, in2);
    ledcWrite(m_chPwm, pwm);
}
