#include "mbed.h"

#define USE_PRINTF (1)

DigitalOut led_blue(PB_5);


//#include <DSPI.h>
#include <OpenBCI_32bit_Library.h>
#include <OpenBCI_32Bit_Library_Definitions.h>

void setup() {
  // Bring up the OpenBCI Board
  board.begin();
}

void loop() {

  if (board.streaming) {
    if (board.channelDataAvailable) {
      // Read from the ADS(s), store data, set channelDataAvailable flag to false
      board.updateChannelData();

      if (board.timeSynced) {
        // Send time synced packet with channel data, current board time, and an accel reading
        //  X axis is sent on sampleCounter % 10 == 7
        //  Y axis is sent on sampleCounter % 10 == 8
        //  Z axis is sent on sampleCounter % 10 == 9
        board.sendChannelDataWithTimeAndAccel();
      } else {
        // Send standard packet with channel data
        board.sendChannelDataWithAccel();
      }
      /*
      if (board.timeSynced) {
        board.sendChannelDataWithTimeAndRawAux();
      } else {
        // Send standard packet with channel data
        board.sendChannelDataWithRawAux();
      }
      */
    }
  }
  // Check the serial port for new data
  if (board.hasDataSerial0()) {
    // Read one char and process it
    board.processChar(board.getCharSerial0());
  }
}



int main() 
{  
  setup();

  while (1) loop();
}

