// interface for Uart implemented with mbed framework

#ifndef __PlatformUart__
#define __PlatformUart__

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#include <stddef.h>
#include <stdint.h>


class platformUart_Dummy {
public:
  int write(int data) { return 0; }
  //int write(char* str);

    size_t print(const char[]) { return 0; }
    size_t print(char) { return 0; }
    size_t print(unsigned char, int = DEC) { return 0; }
    size_t print(int, int = DEC) { return 0; }
    size_t print(unsigned int, int = DEC) { return 0; }
    size_t print(long, int = DEC) { return 0; }
    size_t print(unsigned long, int = DEC) { return 0; }
    size_t print(double, int = 2) { return 0; }

    size_t println(const char[]) { return 0; }
    size_t println(char) { return 0; }
    size_t println(unsigned char, int = DEC) { return 0; }
    size_t println(int, int = DEC) { return 0; }
    size_t println(unsigned int, int = DEC) { return 0; }
    size_t println(long, int = DEC) { return 0; }
    size_t println(unsigned long, int = DEC) { return 0; }
    size_t println(double, int = 2) { return 0; }
    size_t println(void) { return 0; }
    
    unsigned char available() { return 0; }
    unsigned char read() { return 0; }
		void begin(int baudrate) {};
			
		operator int() const { return 0; }
  
};

class platformUart {
public:
  int write(int data);
  //int write(char* str);

    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);

    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(void);
    
    unsigned char available();
    unsigned char read();
		void begin(int baudrate) {}
    
    platformUart();
    
private:
    
  size_t printNumber(unsigned long n, uint8_t base);
  size_t printFloat(double number, uint8_t digits);
  
};

#endif
