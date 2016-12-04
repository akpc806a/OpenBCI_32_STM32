// interface for SPI implemented with mbed framework

#ifndef __PlatformSPI__
#define __PlatformSPI__

#define	DSPI_MODE0	0	// CKP = 0 CKE = 1 ====> POL=0 PHA=0
#define	DSPI_MODE1	1	// CKP = 0 CKE = 0 ====> POL=0 PHA=1
#define	DSPI_MODE2	2	// CKP = 1 CKE = 1 ====> POL=1 PHA=0
#define	DSPI_MODE3	3	// CKP = 1 CKE = 0 ====> POL=1 PHA=1

class DSPI0 {
public:
  void begin();
  void setSpeed(int speed_hz); 
  void setMode(int mode); 
  int transfer(int data);
private:

};

#endif