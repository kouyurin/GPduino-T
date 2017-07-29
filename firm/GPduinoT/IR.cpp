#include <Arduino.h>
#include "GPduinoT.h"

// pulse length [usec]
#define IR_T_HEAD     6600      // header
#define IR_T_SHORT    550       // short space/mark
#define IR_T_LONG     1650      // long space/mark
#define IR_T_ERROR    300       // error margin TODO
//#define IR_T_INTERVAL 91000

// IR code (1 byte data)
#define IR_CODE_BATTLE_ROYAL    0x669A
#define IR_CODE_TEAM_A_15P      0x9589 // TODO
#define IR_CODE_TEAM_A_20P      0x9589
#define IR_CODE_TEAM_A_30P      0x9589 // TODO
#define IR_CODE_TEAM_B_15P      0xB7AB // TODO
#define IR_CODE_TEAM_B_20P      0xB7AB
#define IR_CODE_TEAM_B_30P      0xB7AB // TODO

static const uint16_t IR_CODE[3][3]={
    {IR_CODE_BATTLE_ROYAL, IR_CODE_BATTLE_ROYAL, IR_CODE_BATTLE_ROYAL},
    {IR_CODE_TEAM_A_15P,   IR_CODE_TEAM_A_20P,   IR_CODE_TEAM_A_30P},
    {IR_CODE_TEAM_B_15P,   IR_CODE_TEAM_B_20P,   IR_CODE_TEAM_B_30P}
};

#ifdef DEMO_MB_OSAKA  // IR code for Color LED Light control
static const uint8_t LIGHT_CODE[9]={
//  ON    BLUE  GREEN RED   CYAN YELLOW MAGENTA WHITE OFF
    0x07, 0x0A, 0x08, 0x09, 0x10, 0x11,  0x12,  0x0B, 0x06
};

#define TAP_CODE_LEN  19
static const uint16_t TAP_CODE_ON[TAP_CODE_LEN]={
  // LEADER
  2600, 2600,
  //      0          0          1          1
  850,  850, 850,  850, 850, 1850, 850, 1850,
  //      1          1          0          0
  850, 1850, 850, 1850, 850,  850, 850,  850, 850    
};
static const uint16_t TAP_CODE_OFF[TAP_CODE_LEN]={
  // LEADER
  2600, 2600,
  //      0          1          0          0
  850,  850, 850, 1850, 850,  850, 850,  850,
  //      1          0          1          1
  850, 1850, 850,  850, 850, 1850, 850, 1850, 850    
};
#endif

// IR modulation frequency [kHz]
#define IR_FREQ         38000

// PWM resolution [bit]
#define IR_RESOLUTION   8

// PWM duty 50%
#define IR_DUTY_50      128

// Timer devider for 1usec (@80MHz)
#define TIMER_DIV_1US   80

// convert usec -> clocks (@80MHz)
// #define USEC2CLOCK(us)    ((us) * 80L)

// global object for ISR
static IR* s_ir;

// timer interrupt ISR
static void IRAM_ATTR timer_ISR(void)
{
    const static DRAM_ATTR void* dNULL = NULL;
    
     //Serial.println("timer_ISR");
    
    if(s_ir != dNULL){
        s_ir->onTimer();
    }
}

// external interrupt ISR
static void IRAM_ATTR extint_ISR(void)
{
    const static DRAM_ATTR void* dNULL = NULL;
    //Serial.println("extint_ISR");
    
    if(s_ir != dNULL){
        s_ir->onExtInt();
    }
}

// constructor
// pinRecv: pin number of IR Receiver
// pinLed: pin number of IR LED
// chLed: channel number of IR LED PWM
// chTimer: channel number of Timer
IR::IR(int pinRecv, int pinLed, int chLed, int chTimer)
{
    m_pinRecv = pinRecv;
    m_pinLed = pinLed;
    m_chLed = chLed;
    m_chTimer = chTimer;

    // initialize variables
    m_team = BATTLE_ROYAL;
    m_gunClass = GUN_20P;
    m_onHit = NULL;
    m_sendCnt = 0;
    m_recvCnt = 0;
    m_isShooting = false;
    m_isReceiving = false;
    m_hasReceived = false;
}

// register a team
// team : TEAM_A / TEAM_B / BATTLE_ROYAL
void IR::setTeam(int team)
{
    m_team = team;
}

// register the gun class
// class : GUN_15P / GUN_20P / GUN_30P
void IR::setGun(int gunClass)
{
    m_gunClass = gunClass;
}

// register a callback invoked on hit
// team : TEAM_A / TEAM_B / BATTLE_ROYAL
// weapon : MAIN_CANNON / MACHINE_GUN
void IR::setOnHit(void (*onHit)(int team, int gun))
{
    m_onHit = onHit;
}

// setup the IR system
// return : success or failure
bool IR::begin()
{
    noInterrupts();
    
    // setup the external interrupt for receive
    s_ir = this;
    pinMode(m_pinRecv, INPUT);
    
    attachInterrupt(m_pinRecv, extint_ISR, CHANGE);
    
    // setup IR modulation frequency and turn off IR LED
    ledcAttachPin(m_pinLed, m_chLed);
    ledcSetup(m_chLed, IR_FREQ, IR_RESOLUTION);

    // setup timer interrupt for IR LED code
    m_timer = timerBegin(m_chTimer, TIMER_DIV_1US, true);
    timerAttachInterrupt(m_timer, &timer_ISR, true);
    timerAlarmWrite(m_timer, IR_T_HEAD, true);
    timerStart(m_timer);
    timerAlarmEnable(m_timer);
    
    interrupts();
    
    return true;
}

// callback invoked from external interrupt handler (for receive)
void IRAM_ATTR IR::onExtInt()
{
    const static DRAM_ATTR bool TRUE = true;
    const static DRAM_ATTR bool FALSE = false;
    const static DRAM_ATTR int dHIGH = HIGH;
    const static DRAM_ATTR int ZERO = 0;
    const static DRAM_ATTR int dIR_T_HEAD = IR_T_HEAD;
    
    //Serial.print("IR::onExtInt ");
    
    // pulse level (HIGH or LOW)
    int recv = digitalRead(m_pinRecv);
    
    // pulse width [usec]
    uint32_t t = micros();
    uint16_t dt = (uint16_t)(t - m_timePrev);
    m_timePrev = t;
    //Serial.print(recv);
    //Serial.print(" ");
    //Serial.print(dt);
    //Serial.print(" ");
    
    // if header mark, start receiving data
    if( (recv == dHIGH) && time_match(dt, dIR_T_HEAD))
    {
        //Serial.print("header ");
        m_recvCnt = ZERO
        ;
        m_isReceiving = TRUE;
    }
    // receive data
    else if(m_isReceiving)
    {
        //Serial.print(m_recvCnt);
        m_recvBuff[m_recvCnt] = dt;
        m_recvCnt++;
        
        // byte data end?
        if(m_recvCnt >= IR_RECV_BUFF_SIZE){
            //Serial.println("m_hasReceived");
            m_isReceiving = FALSE;
            m_hasReceived = TRUE;
        }
    }
    //Serial.println("");
}

// please call this function in main loop
void IR::loop()
{
    // data has been received?
    if(!m_hasReceived) return;
    m_hasReceived = false;
    
    Serial.print("IR::loop decode ");
    for(int i=0;i<16;i++)
    {
        Serial.print(m_recvBuff[i]);
        Serial.print(" ");
    }
    
    // decode a byte data
    uint16_t data = 0;
    for(int i=0;i<16;i++)
    {
        // pulse length
        uint16_t length = m_recvBuff[i];
        //Serial.print(length);
        //Serial.print(" ");
        
        // long pulse -> bit 1
        if(time_match(length, IR_T_LONG))
        {
            Serial.print("L");
            data <<= 1;
            data |= 0x01;
        }
        // short pulse -> bit 0
        else if(time_match(length, IR_T_SHORT))
        {
            Serial.print("S");
            data <<= 1;
        }
        else
        {
            Serial.println("E");
            // invalid data!
            return;
        }
    }
    Serial.println(" Data=");
    Serial.println(data, HEX);
    
    // callback the registerd handler
    int team = BATTLE_ROYAL;
    int gun = GUN_20P;
    switch(data){
    case IR_CODE_BATTLE_ROYAL:
        team = BATTLE_ROYAL;
        gun = GUN_20P;
        break;
//    case IR_CODE_TEAM_A_15P:
//        team = TEAM_A;
//        gun = GUN_15P;
//        break;
    case IR_CODE_TEAM_A_20P:
        team = TEAM_A;
        gun = GUN_20P;
        break;
//    case IR_CODE_TEAM_A_30P:
//        team = TEAM_A;
//        gun = GUN_30P;
//        break;
//    case IR_CODE_TEAM_B_15P:
//        team = TEAM_B;
//        gun = GUN_15P;
//        break;
    case IR_CODE_TEAM_B_20P:
        team = TEAM_B;
        gun = GUN_20P;
        break;
//    case IR_CODE_TEAM_B_30P:
//        team = TEAM_B;
//        gun = GUN_30P;
//        break;
    default:
        return;
    }
    if(m_onHit != NULL){
        m_onHit(team, gun);
    }
}

// shoot a cannon/gun
bool IR::shoot(int code)
{
    //Serial.println("IR::shoot");
    
    if(m_isShooting) return false;
    m_isShooting = true;
    
    // byte data to send
    uint16_t data = IR_CODE[m_team][m_gunClass];
    
    // code the byte data
    m_sendBuff[0] = IR_T_HEAD;
    for(int i=0;i<16;i++){
        bool msb_zero = ((data & 0x8000) == 0);
        m_sendBuff[1+i] = msb_zero ? IR_T_SHORT : IR_T_LONG;
        data <<= 1;
    }
    m_sendSize = IR_SEND_BUFF_SIZE;
    
#ifdef DEMO_MB_OSAKA // IR code for Color LED Light control
    if(code == 0){
        // set NEC format data
        static int cntCode = 0;
        setNECFormat(0x00, LIGHT_CODE[cntCode]);
        cntCode++;
        if(cntCode >= 9) cntCode = 0;
        m_sendSize = IR_SEND_BUFF_SIZE;
    }
    else if(code == 1){
        for(int i=0;i<TAP_CODE_LEN;i++)
        {
            m_sendBuff[i] = TAP_CODE_ON[i];
        }
        m_sendSize = TAP_CODE_LEN;
    }
    else if(code == 2){
        for(int i=0;i<TAP_CODE_LEN;i++)
        {
            m_sendBuff[i] = TAP_CODE_OFF[i];
        }
        m_sendSize = TAP_CODE_LEN;
    }
#endif
    
    m_sendCnt = 0;
    
    // start to output a header mark pulse
    ledcWrite(m_chLed, IR_DUTY_50);
    
    // set the timer interrupt
    noInterrupts();
    timerWrite(m_timer, 0);
    timerAlarmWrite(m_timer, m_sendBuff[0], true);
    interrupts();
}

// callback invoked from timer interrupt handler (for send)
void IRAM_ATTR IR::onTimer()
{
    const static DRAM_ATTR int ZERO = 0;
    const static DRAM_ATTR int ONE = 0x01;
    const static DRAM_ATTR bool TRUE = true;
    const static DRAM_ATTR bool FALSE = false;
    const static DRAM_ATTR int dIR_DUTY_50 = IR_DUTY_50;
    const static DRAM_ATTR int dIR_SEND_BUFF_SIZE = IR_SEND_BUFF_SIZE;
    const static DRAM_ATTR int dIR_T_HEAD = IR_T_HEAD;
    
    if(!m_isShooting) return;
    
    m_sendCnt++;
    // toggle the pulse output
    if((m_sendCnt & ONE) == ZERO){
        ledcWrite(m_chLed, dIR_DUTY_50);
    }else{
        ledcWrite(m_chLed, ZERO);
    }
    
    // data remain?
    if(m_sendCnt < dIR_SEND_BUFF_SIZE){
        // set the next timer interrupt
        timerAlarmWrite(m_timer, m_sendBuff[m_sendCnt], TRUE);
    }else{
        timerAlarmWrite(m_timer, dIR_T_HEAD, TRUE);
        m_isShooting = FALSE;
    }
}

// indicate wether two time counts match (with error margin)
bool IR::time_match(uint16_t t1, uint16_t t2)
{
    int16_t err = (int16_t)t1 - (int16_t)t2;
    
    if((-IR_T_ERROR < err) && (err < IR_T_ERROR)){
        return true;
    }else{
        return false;
    }
}

// set NEC format data
void IR::setNECFormat(uint8_t customer, uint8_t data)
{
    const uint16_t T = 562;
    
    // Leader
    m_sendBuff[0] = 16 * T;
    m_sendBuff[1] = 16 * T;
    
    // LSB first
    for(int i=0;i<8;i++)
    {
        // Customer Code (1st byte)
        m_sendBuff[2 + 2*i] = T;
        m_sendBuff[2 + 2*i+1] = ((customer & 0x01) == 0x01) ? 3*T : T;
        
        // ~Custormer Code (2nd byte)
        m_sendBuff[18 + 2*i] = T;
        m_sendBuff[18 + 2*i+1] = ((customer & 0x01) == 0x00) ? 3*T : T;
        
        // Data (3rd byte)
        m_sendBuff[34 + 2*i] = T;
        m_sendBuff[34 + 2*i+1] = ((data & 0x01) == 0x01) ? 3*T : T;
        
        // ~Data (4th byte)
        m_sendBuff[50 + 2*i] = T;
        m_sendBuff[50 + 2*i+1] = ((data & 0x01) == 0x00) ? 3*T : T;
        
        customer >>= 1;
        data     >>= 1;
    }
    // last pulse
    m_sendBuff[66] = T;
}
