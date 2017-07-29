#ifndef _SOUND_H_
#define _SOUND_H_

#define SOUND_BUFF_SIZE     512

class Sound
{
public:
    // constructor
    Sound(int chTimer);
    // begin the solenoid driver
    void begin();
    // play sound
    void play(uint32_t segment, uint32_t size);
    // repeat sound
    void repeat(uint32_t segment, uint32_t size);
    // stop sound
    void stop();
    // cyclic procedure. It must be called from main loop()
    void loop();
    // timer handler
    void onTimer();

private:
    int m_chTimer;          // channel number of timer
    
    hw_timer_t *m_timer;    // timer object
    
    bool m_toLoadData;      // data load flag
    bool m_toSkipLoad;      // load skip flag
    
    bool m_isRepeating;     // repeat flag
    bool m_isPlaying;       // play flag
    
    uint8_t m_buffer[SOUND_BUFF_SIZE]; // buffer of sound data
    
    uint32_t m_flashOffset;      // flash memory offset address (seek position)
    uint32_t m_flashSegment;     // flash memory segment address (head of sound data)
    uint32_t m_flashDataSize;    // flash memory data size (size of sound data)
};

#endif
