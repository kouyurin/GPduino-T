#include <Arduino.h>
#include <Wire.h>
#include "Led.h"

// Sub address
#define SUB_ADDR_INPUT      0x00
#define SUB_ADDR_PSC0       0x01
#define SUB_ADDR_PWM0       0x02
#define SUB_ADDR_PSC1       0x03
#define SUB_ADDR_PWM1       0x04
#define SUB_ADDR_LS0        0x05
#define SUB_ADDR_LS1        0x06

// I2C slave address
int Led::m_slaveAddress = 0x60;

// LEDs state
uint16_t Led::m_ledState = 0x5555; // All LEDs off

// set I2C slave address
void Led::setSlaveAddress(int addr)
{
    m_slaveAddress = addr;
}

// constructor
Led::Led(int ch)
{
    m_ch = ch;
}

// begin the LED driver
void Led::begin()
{
    this->turnOff();
}

// turn off
void Led::turnOff()
{
    uint16_t mask = 0x0003 << (m_ch*2);
    uint16_t set  = 0x0001 << (m_ch*2);
    
    m_ledState = (m_ledState & ~mask) | set;
    
    uint8_t data = (m_ch < 4) ? (uint8_t)(m_ledState & 0x00FF) : (uint8_t)(m_ledState >> 8);
    uint8_t addr = (m_ch < 4) ? SUB_ADDR_LS0 : SUB_ADDR_LS1;
    
    Wire.beginTransmission(m_slaveAddress);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
}

// turn on
void Led::turnOn()
{
    uint16_t mask = 0x0003 << (m_ch*2);
    uint16_t set  = 0x0000 << (m_ch*2);
    
    m_ledState = (m_ledState & ~mask) | set;
    
    uint8_t data = (m_ch < 4) ? (uint8_t)(m_ledState & 0x00FF) : (uint8_t)(m_ledState >> 8);
    uint8_t addr = (m_ch < 4) ? SUB_ADDR_LS0 : SUB_ADDR_LS1;
    
    Wire.beginTransmission(m_slaveAddress);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
}

// blink
// cycle: blink cycle (common to all LEDs), (cycle + 1)/38 sec
void Led::blink(int cycle)
{
    // blink cycle (common to all LEDs)
    Wire.beginTransmission(m_slaveAddress);
    Wire.write(SUB_ADDR_PSC0);
    Wire.write(cycle);
    Wire.endTransmission();
    
    // set blink to the LED
    uint16_t mask = 0x0003 << (m_ch*2);
    uint16_t set  = 0x0002 << (m_ch*2);
    
    m_ledState = (m_ledState & ~mask) | set;
    
    uint8_t data = (m_ch < 4) ? (uint8_t)(m_ledState & 0x00FF) : (uint8_t)(m_ledState >> 8);
    uint8_t addr = (m_ch < 4) ? SUB_ADDR_LS0 : SUB_ADDR_LS1;
    
    Wire.beginTransmission(m_slaveAddress);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
}

/*
// set PWM duty
void Led::write(int duty)
{
    m_isBlinking = false;
    ledcWrite(m_ch, duty);
}

// set blink cycle and PWM duty
void Led::blink(int cycle, int duty)
{
    ledcWrite(m_ch, duty);
    m_duty = duty;
    m_toggleBlink = true;
    m_cycleBlink = cycle;
    m_countBlink = 0;
    m_isBlinking = true;
}

// call on 10msec cycle timer
void Led::onTimer()
{
    if(m_isBlinking)
    {
        m_countBlink++;
        if(m_countBlink >= m_cycleBlink)
        {
            m_countBlink = 0;
            m_toggleBlink = !m_toggleBlink;
            if(m_toggleBlink){
                ledcWrite(m_ch, m_duty);
            }else{
                ledcWrite(m_ch, 0);
            }
        }
    }
}
*/
