#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"

#include "Sound.h"

// sample rate[Hz]
#define SAMPLE_RATE     11025
// I2S ch number
#define I2S_NUM         (I2S_NUM_0)

// Timer devider for 1usec (@80MHz)
#define TIMER_DIV_1US   80
// sound data load cycle [usec]
#define LOAD_CYCLE      22000

// global object for ISR
static Sound* s_sound;

// timer interrupt ISR
static void IRAM_ATTR timer_ISR(void)
{
    const static DRAM_ATTR void* dNULL = NULL;

    //Serial.println("timer_ISR");
    
    if(s_sound != dNULL){
        s_sound->onTimer();
    }
}

// constructor
Sound::Sound(int chTimer)
{
    m_chTimer = chTimer;
}

// begin the solenoid driver
void Sound::begin()
{
    m_isRepeating = false;
    m_isPlaying = false;
    
    // setup I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN), // Built-In DAC
        .sample_rate = SAMPLE_RATE,                     // 11.025kHz
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,   // 16bit sample
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,    // Mono Left Only (PIN 26)
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,// I2S format ***TODO
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,       // Interrupt level 1
        .dma_buf_count = 16,    // Buffer size = 16page * 64sample * 2byte(16bit) * 1ch(Mono)
        .dma_buf_len = 64       // = 2048byte
    };
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN);

    // setup SPI Flash 
    spi_flash_init();
    
    // setup timer interrupt
    noInterrupts();
    s_sound = this;
    m_timer = timerBegin(m_chTimer, TIMER_DIV_1US, true);
    timerAttachInterrupt(m_timer, &timer_ISR, true);
    timerAlarmWrite(m_timer, LOAD_CYCLE, true);
    timerStart(m_timer);
    timerAlarmEnable(m_timer);
    interrupts();
}

// play sound
void Sound::play(uint32_t segment, uint32_t size)
{
    m_isPlaying = false;
    m_isRepeating = false;
    
    m_flashSegment = segment;
    m_flashDataSize = size;
    m_flashOffset = 0;
    
    m_toLoadData = false;
    m_toSkipLoad = false;
    
    i2s_start(I2S_NUM);
    
    // push first data from SPI Fhash-ROM to I2S buffer
    //spi_flash_read(m_flashSegment + m_flashOffset, m_buffer, sizeof(m_buffer));
    i2s_write_bytes(I2S_NUM, (const char *)m_buffer, SOUND_BUFF_SIZE, 0); // timeout 100msec
    m_flashOffset += SOUND_BUFF_SIZE;

    //spi_flash_read(m_flashSegment + m_flashOffset, m_buffer, sizeof(m_buffer));
    i2s_write_bytes(I2S_NUM, (const char *)m_buffer, SOUND_BUFF_SIZE, 0); // timeout 100msec
    m_flashOffset += SOUND_BUFF_SIZE;
    m_isPlaying = true;
}

// repeat sound
void Sound::repeat(uint32_t segment, uint32_t size)
{
    this->play(segment, size);
    
    m_isRepeating = true;
}

// stop sound
void Sound::stop()
{
    m_isPlaying = false;
    m_isRepeating = false;
    

    i2s_stop(I2S_NUM);
}

int toggle = 0;

// cyclic procedure. It must be called from main loop()
void Sound::loop()
{

    if(m_toLoadData){
        m_toLoadData = false;

        // skip this cycle?
        if(m_toSkipLoad){
            m_toSkipLoad = false;
            return;
        }
        
        // push next data from SPI Fhash-ROM to I2S buffer
        //noInterrupts();
        //esp_intr_noniram_disable();
        ESP_INTR_DISABLE(10);
        spi_flash_read(m_flashSegment + m_flashOffset, m_buffer, sizeof(m_buffer));
        ESP_INTR_ENABLE(10);
        //interrupts();
        //esp_intr_noniram_enable();
        int len = i2s_write_bytes(I2S_NUM, (const char *)m_buffer, SOUND_BUFF_SIZE, 0); // non blocking
        m_flashOffset += len;
        
        // when I2S buffer is busy, skip next cycle
        if( len <= SOUND_BUFF_SIZE){
/*
            m_toSkipLoad = true;
            
            Serial.print("Busy ");
            Serial.print(len);
            Serial.print(" @");
            Serial.println(m_flashOffset);
*/
        }
        
        // end of sound data?
        if(m_flashOffset > m_flashDataSize){
            // repeat?
            if(m_isRepeating){
                m_flashOffset = 0;
            }
            // stop?
            else{
                stop();
            }
        }
    }
}

// timer handler
void IRAM_ATTR Sound::onTimer()
{
    const static DRAM_ATTR bool TRUE = true;
    
    if(m_isPlaying){
        m_toLoadData = TRUE;
    }
}
