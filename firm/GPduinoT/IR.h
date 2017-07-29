#ifndef _IR_H_
#define _IR_H_

#include <stdint.h>

// send & receive buffer size
#ifdef DEMO_MB_OSAKA
#define IR_SEND_BUFF_SIZE (67)  // for NEC format
#else
#define IR_SEND_BUFF_SIZE (16+1)
#endif
#define IR_RECV_BUFF_SIZE (16)

// IR LEDs & receiver for battle
class IR
{
public:
    // constructor
    IR(int pinRecv, int pinLed, int chLed, int chTimer);
    // register the team (A Team / B Team / Battle Royal)
    void setTeam(int team);
    // register the gun class (15P / 20P / 30P)
    void setGun(int gunClass);
    // register a callback invoked on hit
    void setOnHit(void (*onHit)(int team, int gun));
    // setup the IR system
    bool begin();
    // please call this function in main loop
    void loop();
    // shoot the gun
    bool shoot(int code);
    
    // callback invoked from timer interrupt handler (for send)
    void onTimer();
    // callback invoked from external interrupt handler (for receive)
    void onExtInt();
    
private:
    // pin number of IR Receiver
    int m_pinRecv;
    // pin number of IR LED
    int m_pinLed;
    // channel number of IR LED PWM
    int m_chLed;
    // channel number of Timer
    int m_chTimer;
    // my team (A Team / B Team / Battle Royal)
    int m_team;
    // my gun class (15P / 20P / 30P)
    int m_gunClass;
    // callback invoked on hit
    void (*m_onHit)(int team, int gun);
    // send buffer
    uint16_t m_sendBuff[IR_SEND_BUFF_SIZE];
    // receive buffer
    uint16_t m_recvBuff[IR_RECV_BUFF_SIZE];
    // send counter
    int m_sendCnt;
    // receive counter
    int m_recvCnt;
    // flag indicating whether the IR system is shooting a gun now
    bool m_isShooting;
    // flag indicating whether the IR system is receiving data now
    bool m_isReceiving;
    // flag indicating whether the IR system has already received data
    bool m_hasReceived;
    // time count of the last external interrupt
    uint32_t m_timePrev;
    // timer object
    hw_timer_t *m_timer;
    // ir send size;
    int m_sendSize;
    
    // indicate wether two time counts match (with error margin)
    bool time_match(uint16_t t1, uint16_t t2);
    
    // set NEC format IR data
    void setNECFormat(uint8_t customer, uint8_t data);
    // set free format IR data
    void setFreeIR(uint16_t *data, int len);
};

#endif
