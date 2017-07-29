#ifndef _COMMON_H_
#define _COMMON_H_

// for demo
#define DEMO_MB_OSAKA   // for Makers Bazaar Osaka vol.4

// team
#define BATTLE_ROYAL    0
#define TEAM_A          1
#define TEAM_B          2

#define GUN_15P         0
#define GUN_20P         1
#define GUN_30P         2

// Hit Point
#define HIT_POINT_FULL  6

// Pin
#define PIN_MOTOR_RIGHT_IN1     0
#define PIN_MOTOR_RIGHT_IN2     2
#define PIN_MOTOR_RIGHT_PWM     4
#define PIN_MOTOR_LEFT_IN1      5
#define PIN_MOTOR_LEFT_IN2      18
#define PIN_MOTOR_LEFT_PWM      19
#define PIN_MOTOR_TURRET_IN1    12
#define PIN_MOTOR_TURRET_IN2    13
#define PIN_MOTOR_TURRET_PWM    14
#define PIN_MOTOR_BARREL_IN1    32
#define PIN_MOTOR_BARREL_IN2    25
#define PIN_MOTOR_BARREL_PWM    27

#define PIN_SOLENOID            15
#define PIN_SOUND_OUT           26
#define PIN_LED_IR_GUN          33
#define PIN_ADC_BATTERY         34
#define PIN_IR_RECEIVER         35

// LED channel (0-15)
#define PWM_MOTOR_BARREL        0
#define PWM_MOTOR_TURRET        1
#define PWM_MOTOR_LEFT          2
#define PWM_MOTOR_RIGHT         3
#define PWM_SOLENOID            4
#define PWM_SOUND_OUT           5
#define PWM_LED_IR_GUN          7

// Timer channel (0-3)
#define TIMER_IR_GUN            0
#define TIMER_SOUND             1
#define TIMER_10MS              2

// External LED driver channel
#define LED_HEAD_LIGHT          0
#define LED_SHOOTING            1
#define LED_HIT                 2
#define LED_STATUS_RED          3
#define LED_STATUS_GREEN        4

// LED blink cycle
#define LED_BLINK_CYCLE_1S      38
#define LED_BLINK_CYCLE_0_5S    19
#define LED_BLINK_CYCLE_0_25S   9

// Battery divider ratio
#define BATTERY_DIV_RATIO       3

// Sound data address (flash memory)
#define SOUND_SEG_IDLE          0x100000
#define SOUND_SEG_DRIVING       0x200000
#define SOUND_SEG_CANNON        0x100000 // TODO 0x300000
#define SOUND_SEG_GUN           0x400000
#define SOUND_SEG_HIT           0x500000
#define SOUND_SEG_HALT          0x600000
// TODO
#define SOUND_SIZE_IDLE         0x100000
#define SOUND_SIZE_DRIVING      0x100000
#define SOUND_SIZE_CANNON       (59*1024)  //0x100000
#define SOUND_SIZE_GUN          0x100000
#define SOUND_SIZE_HIT          0x100000
#define SOUND_SIZE_HALT         0x100000

// Timer devider for 1usec (@80MHz)
#define TIMER_DIV_1US       80
// 10msec Timer Cycle [usec]
#define TIMER_CYCLE_10MS    10000

// Shooting action time [10msec]
#define TIME_SHOOTING       60
// Hit action time [10msec]
#define TIME_HIT            120
// Canon loading time [10msec]
#define TIME_LOADING        500
// Action time [10msec]
#define TIME_ACTION         30

// Shooting / Hit action PWM
#define ACTION_PWM          32

#endif
