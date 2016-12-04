// interface for SPI implemented with mbed framework

#include "PlatformUart.h"
#include "mbed.h"

Serial uart(PA_9, PA_10, 115200); // Serial(PinName tx, PinName rx, const char *name=NULL, int baud = MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE);


int platformUart::write(int data)
{
  return uart.putc(data);
}
/*
int platformUart::write(char* str)
{
  return uart.puts(str);
}
*/

size_t platformUart::println(void)
{
  return uart.puts("\r\n");
}

size_t platformUart::print(const char str[])
{
  return uart.puts(str);
}

size_t platformUart::print(char c)
{
  return uart.putc(c);
}

size_t platformUart::print(unsigned char b, int base)
{
  return print((unsigned long) b, base);
}

size_t platformUart::print(int n, int base)
{
  return print((long) n, base);
}

size_t platformUart::print(unsigned int n, int base)
{
  return print((unsigned long) n, base);
}

size_t platformUart::print(long n, int base)
{
  if (base == 0) {
    return write(n);
  } else if (base == 10) {
    if (n < 0) {
      int t = print('-');
      n = -n;
      return printNumber(n, 10) + t;
    }
    return printNumber(n, 10);
  } else {
    return printNumber(n, base);
  }
}

size_t platformUart::print(unsigned long n, int base)
{
  if (base == 0) return write(n);
  else return printNumber(n, base);
}

size_t platformUart::print(double n, int digits)
{
  return printFloat(n, digits);
}


size_t platformUart::println(const char c[])
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t platformUart::println(char c)
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t platformUart::println(unsigned char b, int base)
{
  size_t n = print(b, base);
  n += println();
  return n;
}

size_t platformUart::println(int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t platformUart::println(unsigned int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t platformUart::println(long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t platformUart::println(unsigned long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t platformUart::println(double num, int digits)
{
  size_t n = print(num, digits);
  n += println();
  return n;
}


// Private Methods /////////////////////////////////////////////////////////////

size_t platformUart::printNumber(unsigned long n, uint8_t base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return uart.puts(str);
}

size_t platformUart::printFloat(double number, uint8_t digits) 
{ 
  size_t n = 0;
  
  if (isnan(number)) return print("nan");
  if (isinf(number)) return print("inf");
  if (number > 4294967040.0) return print ("ovf");  // constant determined empirically
  if (number <-4294967040.0) return print ("ovf");  // constant determined empirically
  
  // Handle negative numbers
  if (number < 0.0)
  {
     n += print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  n += print(int_part);

  // platformUart the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n += print("."); 
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    n += print(toPrint);
    remainder -= toPrint; 
  } 
  
  return n;
}


extern void eventSerial();
unsigned char rx_data;
unsigned char rx_avalible;

// Interupt Routine to read in data from serial port
void Rx_interrupt() 
{
    while (uart.readable())
    {
      rx_data = uart.getc();
      rx_avalible = 1;
      
      eventSerial();
    }
}



platformUart::platformUart()
{
  rx_data = 0;
  rx_avalible = 0;
  
  uart.attach(&Rx_interrupt, Serial::RxIrq);
}

unsigned char platformUart::available()
{
  return rx_avalible;
}

unsigned char platformUart::read()
{
  rx_avalible = 0;
  return rx_data;
}




platformUart Serial0;