// interface for SPI implemented with mbed framework

#include "PlatformSPI.h"
#include "mbed.h"

SPI spi_adc(PA_7, PA_6, PA_5); // PinName mosi, PinName miso, PinName sclk, PinName ssel=NC

void DSPI0::begin()
{

}

void DSPI0::setSpeed(int speed_hz)
{
  spi_adc.frequency(speed_hz); 
}

void DSPI0::setMode(int mode)
{
  spi_adc.format(8,mode); 
}

int DSPI0::transfer(int data)
{
  return spi_adc.write(data);
}

