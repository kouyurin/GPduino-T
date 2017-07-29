#ifndef    _DC_MOTOR_H_
#define    _DC_MOTOR_H_

// DC motor driver TB6612FNG class
class DcMotor
{
public:
    // constructor
    DcMotor(int pinIn1, int pinIn2, int pinPwm, int chPwm);
    // begin the DC motor driver
    void begin();
    // drive the motor
    void out(int value);
    
private:
    int m_pinIn1;   // pin number of IN1
    int m_pinIn2;   // pin number of IN2
    int m_pinPwm;   // pin number of PWM
    int m_chPwm;    // channel number of PWM
};

#endif
