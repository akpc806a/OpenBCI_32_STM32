// interface for SPI implemented with mbed framework

#ifndef __PlatformGPIO__
#define __PlatformGPIO__

#include "mbed.h"

#define pinMode(x,y)


#define SD_SS PA_3
#define BOARD_ADS PA_0 
#define DAISY_ADS PA_2
#define LIS3DH_SS PC_5
#define ADS_RST PA_1

#define LIS3DH_DRDY PB_10
#define ADS_DRDY PC_3

void digitalWrite(int pin, unsigned char value);
unsigned char digitalRead(int pin);

#endif