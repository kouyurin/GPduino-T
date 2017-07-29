#include <Wire.h>
#include "GPduinoT.h"

// UDP
UdpComm udpComm;

// State
#define STATE_RUNNING   0
#define STATE_SHOOT     1
#define STATE_HIT       2
#define STATE_HALT      3
int mainState;

// IR Receiver & IR LED
IR ir(PIN_IR_RECEIVER, PIN_LED_IR_GUN, PWM_LED_IR_GUN, TIMER_IR_GUN);

// LED
Led ledHeadLight  (LED_HEAD_LIGHT);
Led ledShooting   (LED_SHOOTING);
Led ledHit        (LED_HIT);
Led ledStatusRed  (LED_STATUS_RED);
Led ledStatusGreen(LED_STATUS_GREEN);

// DC Motor
DcMotor motorBarrel(
    PIN_MOTOR_BARREL_IN1, PIN_MOTOR_BARREL_IN2,
    PIN_MOTOR_BARREL_PWM, PWM_MOTOR_BARREL);
DcMotor motorTurret(
    PIN_MOTOR_TURRET_IN1, PIN_MOTOR_TURRET_IN2,
    PIN_MOTOR_TURRET_PWM, PWM_MOTOR_TURRET);
DcMotor motorLeft(
    PIN_MOTOR_LEFT_IN1, PIN_MOTOR_LEFT_IN2,
    PIN_MOTOR_LEFT_PWM, PWM_MOTOR_LEFT);
DcMotor motorRight(
    PIN_MOTOR_RIGHT_IN1, PIN_MOTOR_RIGHT_IN2,
    PIN_MOTOR_RIGHT_PWM, PWM_MOTOR_RIGHT);

// Battery checker
Battery battery(PIN_ADC_BATTERY, BATTERY_DIV_RATIO);

// Solenoid
Solenoid solenoid(PIN_SOLENOID, PWM_SOLENOID);

// Sound
Sound sound(TIMER_SOUND);

// 10msec Timer
hw_timer_t *timer10ms;
// 10msec Timer flag
bool flag10ms;

// Counter for shooting LED
int cntShootingLED;
// Counter for shooting action
int cntShootingAction;

// Counter for hit LED
int cntHitLED;
// Counter for hit action
int cntHitAction;

// Counter for loading time
int cntLoadingTime;

// Crawler control
int ctrlFB;  // Forward-Backword
int ctrlLR;  // Left-Right

// Hit Point
int hitPoint;

// Battery voltage check
void battery_check()
{
    static char txbuff[256];
    
    if(!udpComm.isReady()) return;
    
    static int cnt2 = 0;

    unsigned short Vbat_ave = battery.getAdc() >> 2;
    //Serial.print("Vbat_ave");Serial.println(Vbat_ave);

    // send battery voltage value every about 1 sec
    cnt2++;
    if(cnt2 >= 100)
    {
        cnt2=0;
        
        txbuff[0]='#';
        txbuff[1]='B';
        Uint16ToHex(&txbuff[2], Vbat_ave, 3);
        txbuff[5]='$';
        txbuff[6]='\0';
        udpComm.send(txbuff);
        Serial.print("battery_check ");
        Serial.println(Vbat_ave);
    }
}

// Crawler control
void crawler_control()
{
    int r = (int)(ctrlFB - ctrlLR)*8;
    int l = (int)(ctrlFB + ctrlLR)*8;

    motorLeft.out(l);
    motorRight.out(r);
}

// handler called when this tank is hit
void onHit(int team, int gunClass)
{
    Serial.print("onHit ");
    Serial.print(team);
    Serial.print(" ");
    Serial.println(gunClass);
    
    // Hit Point decrement
    if(hitPoint > 0) hitPoint--;
    switch(hitPoint){
    case 5:
        // Green:Blink(1s) Red:Off
        ledStatusGreen.blink(LED_BLINK_CYCLE_1S);
        ledStatusRed.turnOff();
        break;
    case 4:
        // Green:Blink(0.5s) Red:Off
        ledStatusGreen.blink(LED_BLINK_CYCLE_0_5S);
        ledStatusRed.turnOff();
        break;
    case 3:
        // Green:Off Red:On
        ledStatusGreen.turnOff();
        ledStatusRed.turnOn();
        break;
    case 2:
        // Green:Off Red:Blink(1s)
        ledStatusGreen.turnOff();
        ledStatusRed.blink(LED_BLINK_CYCLE_1S);
        break;
    case 1:
        // Green:Off Red:Blink(0.5s)
        ledStatusGreen.turnOff();
        ledStatusRed.blink(LED_BLINK_CYCLE_0_5S);
        break;
    case 0: // Crush!!
        // Green:Off Red:Off
        ledStatusGreen.turnOff();
        ledStatusRed.turnOff();
        break;
    }
    
    // LED
    if(hitPoint > 0){
        ledHit.turnOn();
        cntHitLED = TIME_HIT;
    }else{
        ledHit.blink(LED_BLINK_CYCLE_1S);
        ledHeadLight.turnOff();
    }
    
    // Hit action
    ctrlLR = ACTION_PWM;
    ctrlFB = 0;
    crawler_control();
    cntHitAction = TIME_HIT;
    cntShootingAction = 0;
    
    // TODO
    //digitalWrite(17, LOW);
    //delay(1000);
    //digitalWrite(17, HIGH);
}

// 10msec cycle timer handler
void IRAM_ATTR on10msTimer()
{
    const static DRAM_ATTR bool TRUE = true;
    flag10ms = TRUE;
}

// initialize
void setup()
{
    // Variables
    mainState = STATE_RUNNING;
    flag10ms = false;
    ctrlFB = 0;
    ctrlLR = 0;
    hitPoint = HIT_POINT_FULL;
    cntShootingLED = 0;
    cntShootingAction = 0;
    cntHitLED = 0;
    cntHitAction = 0;
    cntLoadingTime = 0;
    
    //delay(100); // This is necessary! I can't understand why
    
    // Serial
    Serial.begin(115200);
    Serial.println("setup() start!");
    
    // I2C
    Wire.begin();
    
    // UDP communication
    udpComm.beginAP(NULL, "12345678");
    udpComm.onReceive = UdpComm_callback;
    
    // Sound
    sound.begin();
    
    // IR Receiver & IR LED
    ir.setTeam(BATTLE_ROYAL);
    ir.setGun(GUN_20P);
    ir.setOnHit(onHit);
    ir.begin();
    
    // LED
    ledHeadLight.begin();
    ledShooting.begin();
    ledHit.begin();
    ledStatusRed.begin();
    ledStatusGreen.begin();
    
    ledHeadLight.turnOn();    // Head Light LED:   ON
    ledShooting.turnOff();    // Shooting LED:     OFF
    ledHit.turnOff();         // Hit LED:          OFF
    ledStatusRed.turnOff();   // Status Red LED:   OFF
    ledStatusGreen.turnOn();  // Status Green LED: ON
/*
    // TEST
    ledHeadLight.turnOn();
    ledShooting.blink(LED_BLINK_CYCLE_1S);
    ledHit.turnOn();
    ledStatusRed.turnOn();
    ledStatusGreen.blink(LED_BLINK_CYCLE_1S);
*/
    
    // DC Motor
    motorBarrel.begin();
    motorTurret.begin();
    motorLeft.begin();
    motorRight.begin();

    // Battery checker
    battery.begin();
    
    // Solenoid
    solenoid.begin();
    solenoid.out(512); // TODO
    
    // 10msec Timer
    noInterrupts();
    timer10ms = timerBegin(TIMER_10MS, TIMER_DIV_1US, true);
    timerAttachInterrupt(timer10ms, &on10msTimer, true);
    timerAlarmWrite(timer10ms, TIMER_CYCLE_10MS, true);
    timerStart(timer10ms);
    timerAlarmEnable(timer10ms);
    interrupts();
    
    // TEST TODO
    pinMode(16, INPUT_PULLUP);
    pinMode(17, OUTPUT);
    digitalWrite(17, HIGH);
    
    Serial.println("setup() end!");
}

int io0_last = HIGH;

// main loop
void loop()
{
    static int cnt = 0;

    // UDP communication
    udpComm.loop();
    
    // IR receive
    ir.loop();
    
    // Sound data load
    sound.loop();
    
    // TODO (for test)
    int io0 = digitalRead(16);
    if((io0 == LOW) && (io0_last == HIGH)){
        // IR shoot
        ir.shoot(2);
        // Shooting LED
        ledShooting.turnOn();
        // Recoil action
        ctrlLR = 0;
        ctrlFB = -ACTION_PWM;
        crawler_control();
        // time counter
        cntShootingAction = TIME_SHOOTING;
        cntShootingLED = TIME_SHOOTING;
        cntLoadingTime = TIME_LOADING;
        cntHitAction = 0;
        delay(50);
        sound.play(SOUND_SEG_CANNON, SOUND_SIZE_CANNON);
    }
    io0_last = io0;
    
    // 10msec timer
    if(flag10ms){
        flag10ms = false;
        
        // battery check
        battery_check();
        
        // Hit action time
        if(cntHitAction > 0){
            cntHitAction--;
            if(cntHitAction == TIME_ACTION*3){
                ctrlLR = -ACTION_PWM;
                ctrlFB = 0;
                crawler_control();
            }
            else if(cntHitAction == TIME_ACTION*2){
                ctrlLR = ACTION_PWM;
                ctrlFB = 0;
                crawler_control();
            }
            else if(cntHitAction == TIME_ACTION*1){
                ctrlLR = -ACTION_PWM;
                ctrlFB = 0;
                crawler_control();
            }
            else if(cntHitAction <= 0){
                ctrlLR = 0;
                ctrlFB = 0;
                crawler_control();
            }
        }
        // Hit LED lighting time
        if(cntHitLED > 0){
            cntHitLED--;
            if(cntHitLED <= 0){
                ledHit.turnOff();
            }
        }
        // Shooting action time
        if(cntShootingAction > 0){
            cntShootingAction--;
            if(cntShootingAction == TIME_ACTION){
                ctrlLR = 0;
                ctrlFB = ACTION_PWM;
                crawler_control();
            }
            else if(cntShootingAction <= 0){
                ctrlLR = 0;
                ctrlFB = 0;
                crawler_control();
            }
        }
        // Shooting LED lighting time
        if(cntShootingLED > 0){
            cntShootingLED--;
            if(cntShootingLED <= 0){
                ledShooting.turnOff();
            }
        }
        // Canon loading time
        if(cntLoadingTime > 0){
            cntLoadingTime--;
        }
    }

    delay(1);
}

// UDP command received callback
// buff: command string buffer
void UdpComm_callback(char* buff)
{
    unsigned short val;
    int sval, sval2;
    int deg;
    int ch;
    int i;
    
    //Serial.print("udpComm_callback:");Serial.println(buff);
    
    if(buff[0] != '#') return;
    buff++;

    // TODO
    //if(hitPoint == 0) return; // No hit point is left
    
    switch(buff[0])
    {
    /* D-Command (Crawler Drive Forward/Backward)
       format: #Dxx$
       xx: 0->Stop, Positive->Forward, Negative->Backward
     */
    case 'D':
        // When shooting or hit, D-Command is invalid
        if( cntShootingAction > 0 ) break;
        if( cntHitAction > 0 ) break;
        
        if( HexToUint16(&buff[1], &val, 2) != 0 ) break;
        sval = (int)((signed char)val);
        
        ctrlFB = sval;
        crawler_control();
        break;
        
    /* T-Command (Crawler Turn Right/Left)
       format: #Txxn$
       n: ignored (this parameter is for racing car controller)
       xx: 0->Neutral, Positive->Right, Negative->Left
     */
    case 'T':
        // When shooting or hit, T-Command is invalid
        if( cntShootingAction > 0 ) break;
        if( cntHitAction > 0 ) break;
        
        if( HexToUint16(&buff[1], &val, 2) != 0 ) break;
        sval = (int)((signed char)val);
        
        ctrlLR = sval;
        crawler_control();
        break;
        
    /* G-Command (Barrel Up/Down)
     */
    case 'G':
        if( HexToUint16(&buff[1], &val, 2) != 0 ) break;
        sval = (int)((signed char)val);
        
        if(sval > 64){
            motorBarrel.out(512);
        }else if(sval < -64){
            motorBarrel.out(-512);
        }else{
            motorBarrel.out(0);
        }
        break;
        
    /* R-Command (Turret Turn Right/Left)
     */
    case 'R':
        if( HexToUint16(&buff[1], &val, 2) != 0 ) break;
        sval = (int)((signed char)val);
        
        if(sval > 64){
            motorTurret.out(512);
        }else if(sval < -64){
            motorTurret.out(-512);
        }else{
            motorTurret.out(0);
        }
        break;
        
    /* F-Command (Fire)
     */
    case 'F':
        if(buff[1] >= '1'){
#ifndef DEMO_MB_OSAKA // No limit for rapid-fire
            if(cntLoadingTime == 0){
#endif
                int code = (int)(buff[1] - '1');
                // IR shoot
                ir.shoot(code);
                // Shooting LED
                ledShooting.turnOn();
                // Recoil action
                ctrlLR = 0;
                ctrlFB = -ACTION_PWM;
                crawler_control();
                // time counter
                cntShootingAction = TIME_SHOOTING;
                cntShootingLED = TIME_SHOOTING;
                cntLoadingTime = TIME_LOADING;
                cntHitAction = 0;
                // sound
                sound.play(SOUND_SEG_CANNON, SOUND_SIZE_CANNON);
#ifndef DEMO_MB_OSAKA
            }
#endif
        }else if(buff[1] == '0'){
            ;
        }
        break;
    }
}


