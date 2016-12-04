// interface for SPI implemented with mbed framework

#include "PlatformGPIO.h"


DigitalOut adc_SS(BOARD_ADS);
DigitalOut adc_reset(ADS_RST);
DigitalOut sd_SS(SD_SS);
DigitalOut daisy_SS(DAISY_ADS);
DigitalOut accel_SS(LIS3DH_SS);

DigitalIn adc_ready(ADS_DRDY);
DigitalIn accel_ready(LIS3DH_DRDY);

void digitalWrite(int pin, unsigned char value)
{
  if (pin == BOARD_ADS)
    adc_SS = value;
  else
  if (pin == ADS_RST)
    adc_reset = value;
  else
  if (pin == SD_SS)
    sd_SS = value;
  else
  if (pin == DAISY_ADS)
    daisy_SS = value;
  else
  if (pin == LIS3DH_SS)
    accel_SS = value;
}

unsigned char digitalRead(int pin)
{
  if (pin == ADS_DRDY)
    return adc_ready;
  else
  if (pin == LIS3DH_DRDY)
    return accel_ready;
}
