// these are definitions to port OpenBCI code to STM32 mbed framework
//#define DSPI0    int

#define boolean  unsigned char
#define byte     unsigned char
  
#define __USER_ISR

#define HIGH 1
#define LOW 0

#include "mbed.h"
#define delay(ms) wait_ms(ms)
#define delayMicroseconds(us) wait_us(us)

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#include "PlatformUart.h"
extern platformUart Serial0;
extern platformUart_Dummy  Serial1;

#include "PlatformSPI.h"

#include "PlatformGPIO.h"

// TODO: SD card stuff
#define stampSD(x)
#define writeDataToSDcard(x)
#define setupSDcard(x) 1
#define closeSDfile(x) 0

#define setIntVector(x,y)
#define setIntPriority(x,y,z)
#define clearIntFlag(x)
//#define setIntEnable(x)

#define PORTA 0
#define _EXTERNAL_4_IRQ 0

#define millis() time(NULL)

#define OPENBCI_SOP_SYMBOL 0xA0//'A'