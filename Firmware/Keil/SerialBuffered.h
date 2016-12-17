#pragma once

// This is a buffered serial reading class, using the serial interrupt introduced in mbed library version 18 on 17/11/09

// In the simplest case, construct it with a buffer size at least equal to the largest message you 
// expect your program to receive in one go.

class SerialBuffered : public RawSerial
{
public:
    SerialBuffered( size_t bufferSize, PinName tx, PinName rx, int baud );
    virtual ~SerialBuffered();
    
    int getc();     // will block till the next character turns up, or return -1 if there is a timeout
    
    int readable(); // returns 1 if there is a character available to read, 0 otherwise
    
    void setTimeout( float seconds );    // maximum time in seconds that getc() should block 
                                         // while waiting for a character
                                         // Pass -1 to disable the timeout.
    
    size_t readBytes( uint8_t *bytes, size_t requested );    // read requested bytes into a buffer, 
                                                             // return number actually read, 
                                                             // which may be less than requested if there has been a timeout
    

private:
    
    void handleInterrupt();
    
   
    uint8_t *m_buff;            // points at a circular buffer, containing data from m_contentStart, for m_contentSize bytes, wrapping when you get to the end
    uint16_t  m_contentStart;   // index of first bytes of content
    uint16_t  m_contentEnd;     // index of bytes after last byte of content
    uint16_t m_buffSize;
    float m_timeout;
    Timer m_timer;

};