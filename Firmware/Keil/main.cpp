#include "mbed.h"

#define USE_PRINTF (1)

DigitalOut led_blue(PB_5);



#include "OpenBCI_32_Daisy.h"

//------------------------------------------------------------------------------
void setup(void);
void loop();
void loadChannelSettings(char c);
void eventSerial();
void getCommand(char token);
void sendEOT();
void loadChannelSettings(char c);
void writeChannelSettings_maintainRunningState(char chan);
void setChannelsToDefaultSetting();
void loadLeadOffSettings(char c);
char getChannelNumber(char n);
void changeChannelState_maintainRunningState(byte chan, int start);
void activateAllChannelsToTestCondition(byte testInputCode, byte amplitudeCode, byte freqCode);
int changeChannelLeadOffDetect_maintainRunningState(char chan);
void sendDefaultChannelSettings();
boolean stopRunning(void);
boolean startRunning(int OUT_TYPE);
void printRegisters();
void startFromScratch();


//------------------------------------------------------------------------------
//  << SD CARD BUSINESS >>
boolean SDfileOpen = false;
char fileSize = '0';  // SD file size indicator
int blockCounter = 0;
//------------------------------------------------------------------------------
//  << OpenBCI BUSINESS >>
boolean is_running = false;    // this flag is set in serialEvent on reciept of ascii prompt
OpenBCI_32_Daisy OBCI;         //Uses SPI bus and pins to say data is ready.
// these are used to change individual channel settings from PC
char currentChannelToSet;      // keep track of what channel we're loading settings for
boolean getChannelSettings = false; // used to receive channel settings command
int channelSettingsCounter;    // used to retrieve channel settings from serial port
int leadOffSettingsCounter;
boolean getLeadOffSettings = false;
// these are all subject to the radio requirements: 31byte max packet length (maxPacketLength - 1 for packet checkSum)
#define OUTPUT_NOTHING (0)        // quiet
#define OUTPUT_8_CHAN (1)         // not using Daisy module
#define OUTPUT_16_CHAN (2)        // using Daisy module
int outputType = OUTPUT_8_CHAN;   // default to 8 channels

//------------------------------------------------------------------------------
//  << LIS3DH Accelerometer Business >>
boolean addAccelToSD = false;   // this flag get's set and cleared in the code below
//------------------------------------------------------------------------------
//  << PUT FILTER STUFF HERE >>
boolean useFilters = false;     // add DSP if you like, and set this true to turn them on
//------------------------------------------------------------------------------
//  << BASIC BOARD STUFF >>
int LED = 11;     // alias for the blue LED
int PGCpin = 12;  // PGC pin goes high when PIC is in bootloader mode
//------------------------------------------------------------------------------
//  << AUXILIARY DATA STUFF >>
boolean addAuxToSD = false;     // use this to write auxiliary data to SD if you like
//------------------------------------------------------------------------------
void setup(void) {

  pinMode(LED, OUTPUT); digitalWrite(LED, HIGH);   // blue LED
  pinMode(PGCpin, OUTPUT); digitalWrite(PGCpin, LOW); // used to tell RFduino if we are in bootloader mode NOT IMPLEMENTED
  delay(1000);            // take a break

  startFromScratch();     // initialize OpenBCI, read device IDs
  // you can set EITHER useAccel or useAux to true
  // if you want both, you MUST set and clear one of the variables every sample
  OBCI.useAccel = true;  // option to add/remove accelerometer data to stream
  OBCI.useAux = false;     // option to add/remove auxiliary data to stream


}



void loop() {

  if (is_running) {

    while (!(OBCI.isDataAvailable())) {} // wait for DRDY pin...

    OBCI.updateChannelData();            // get the fresh ADS results
    if (OBCI.useAccel && OBCI.LIS3DH_DataAvailable()) {
      OBCI.LIS3DH_updateAxisData();      // fresh axis data goes into the X Y Z
      addAccelToSD = true;
    }
    if (SDfileOpen) {
      writeDataToSDcard(OBCI.sampleCounter); // store data locally
    }

    OBCI.sendChannelData();  // serial fire hose
  }

  eventSerial();

}


// some variables to help find 'burger protocol' commands
int plusCounter = 0;
char testChar;
unsigned long commandTimer;

void eventSerial() {
  while (Serial0.available()) {
    char inChar = (char)Serial0.read();
    
    //getCommand(inChar);    // decode the command

    if (getChannelSettings) { // if we just got an 'x' expect channel setting parameters
      loadChannelSettings(inChar);  // go get em!
    } else if (getLeadOffSettings) { // if we just got a 'z' expect lead-off setting parameters
      loadLeadOffSettings(inChar); // go get em!
    } else {
      getCommand(inChar);    // decode the command
    }
    
/*
    if (plusCounter == 1) { // if we have received the first 'bun'
      testChar = inChar;   // this might be the 'patty', stop laughing
      plusCounter++;       // get ready to look for another 'bun'
      commandTimer = millis();  // don't wait too long!
    }

    if (inChar == '+') { // if we see a 'bun' on the serial
      plusCounter++;    // make a note of it
      if (plusCounter == 3) { // looks like we got a command character
        if (millis() - commandTimer < 5) { // if it's not too late,
          if (getChannelSettings) { // if we just got an 'x' expect channel setting parameters
            loadChannelSettings(testChar);  // go get em!
          } else if (getLeadOffSettings) { // if we just got a 'z' expect lead-off setting parameters
            loadLeadOffSettings(testChar); // go get em!
          } else {
            getCommand(testChar);    // decode the command
          }
        }
        plusCounter = 0;  // get ready for the next one
      }
    }
*/
  }
}


void getCommand(char token) {
  switch (token) {
    //TURN CHANNELS ON/OFF COMMANDS
    case '1':
      changeChannelState_maintainRunningState(1, DEACTIVATE); break;
    case '2':
      changeChannelState_maintainRunningState(2, DEACTIVATE); break;
    case '3':
      changeChannelState_maintainRunningState(3, DEACTIVATE); break;
    case '4':
      changeChannelState_maintainRunningState(4, DEACTIVATE); break;
    case '5':
      changeChannelState_maintainRunningState(5, DEACTIVATE); break;
    case '6':
      changeChannelState_maintainRunningState(6, DEACTIVATE); break;
    case '7':
      changeChannelState_maintainRunningState(7, DEACTIVATE); break;
    case '8':
      changeChannelState_maintainRunningState(8, DEACTIVATE); break;
    case '!':
      changeChannelState_maintainRunningState(1, ACTIVATE); break;
    case '@':
      changeChannelState_maintainRunningState(2, ACTIVATE); break;
    case '#':
      changeChannelState_maintainRunningState(3, ACTIVATE); break;
    case '$':
      changeChannelState_maintainRunningState(4, ACTIVATE); break;
    case '%':
      changeChannelState_maintainRunningState(5, ACTIVATE); break;
    case '^':
      changeChannelState_maintainRunningState(6, ACTIVATE); break;
    case '&':
      changeChannelState_maintainRunningState(7, ACTIVATE); break;
    case '*':
      changeChannelState_maintainRunningState(8, ACTIVATE); break;
    case 'q':
      changeChannelState_maintainRunningState(9, DEACTIVATE); break;
    case 'w':
      changeChannelState_maintainRunningState(10, DEACTIVATE); break;
    case 'e':
      changeChannelState_maintainRunningState(11, DEACTIVATE); break;
    case 'r':
      changeChannelState_maintainRunningState(12, DEACTIVATE); break;
    case 't':
      changeChannelState_maintainRunningState(13, DEACTIVATE); break;
    case 'y':
      changeChannelState_maintainRunningState(14, DEACTIVATE); break;
    case 'u':
      changeChannelState_maintainRunningState(15, DEACTIVATE); break;
    case 'i':
      changeChannelState_maintainRunningState(16, DEACTIVATE); break;
    case 'Q':
      changeChannelState_maintainRunningState(9, ACTIVATE); break;
    case 'W':
      changeChannelState_maintainRunningState(10, ACTIVATE); break;
    case 'E':
      changeChannelState_maintainRunningState(11, ACTIVATE); break;
    case 'R':
      changeChannelState_maintainRunningState(12, ACTIVATE); break;
    case 'T':
      changeChannelState_maintainRunningState(13, ACTIVATE); break;
    case 'Y':
      changeChannelState_maintainRunningState(14, ACTIVATE); break;
    case 'U':
      changeChannelState_maintainRunningState(15, ACTIVATE); break;
    case 'I':
      changeChannelState_maintainRunningState(16, ACTIVATE); break;

    // TEST SIGNAL CONTROL COMMANDS
    case '0':
      activateAllChannelsToTestCondition(ADSINPUT_SHORTED, ADSTESTSIG_NOCHANGE, ADSTESTSIG_NOCHANGE); break;
    case '-':
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_SLOW); break;
    case '=':
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_FAST); break;
    case 'p':
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_DCSIG); break;
    case '[':
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_SLOW); break;
    case ']':
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_FAST); break;

    // SD CARD COMMANDS
    //    5min     15min    30min    1hr      2hr      4hr      12hr     24hr    512blocks
    case 'A': case'S': case'F': case'G': case'H': case'J': case'K': case'L': case 'a':
      fileSize = token; SDfileOpen = setupSDcard(fileSize); //
      break;
    case 'j': // close the file, if it's open
      if (SDfileOpen) {
        SDfileOpen = closeSDfile();
      }
      break;

    // CHANNEL SETTING COMMANDS
    case 'x':  // expect 6 parameters
      if (!is_running) {
        Serial0.println("ready to accept new channel settings");
      }
      channelSettingsCounter = 0;
      getChannelSettings = true; break;
    case 'X':  // latch channel settings
      if (!is_running) {
        Serial0.println("updating channel settings");
      }
      writeChannelSettings_maintainRunningState(currentChannelToSet); break;
    case 'd':  // reset all channel settings to default
      if (!is_running) {
        Serial0.println("updating channel settings to default");
      }
      setChannelsToDefaultSetting(); break;
    case 'D':  // report the default settings
      sendDefaultChannelSettings(); break;

    // LEAD OFF IMPEDANCE DETECTION COMMANDS
    case 'z':  // expect 2 parameters
      if (!is_running) {
        Serial0.println("ready to accept new impedance detect settings");
      }
      leadOffSettingsCounter = 0;  // reset counter
      getLeadOffSettings = true;
      break;
    case 'Z':  // latch impedance parameters
      if (!is_running) {
        Serial0.println("updating impedance detect settings");
      }
      changeChannelLeadOffDetect_maintainRunningState(currentChannelToSet);
      break;

    // DAISY MODULE COMMANDS
    case 'c':  // use 8 channel mode
      if (OBCI.daisyPresent) {
        OBCI.removeDaisy();
      }
      outputType = OUTPUT_8_CHAN;
      break;
    case 'C':  // use 16 channel mode
      if (OBCI.daisyPresent == false) {
        OBCI.attachDaisy();
      }
      if (OBCI.daisyPresent) {
        Serial0.print("16"); outputType = OUTPUT_16_CHAN;
      } else {
        Serial0.print("8"); outputType = OUTPUT_8_CHAN;
      }
      sendEOT();
      break;

    // STREAM DATA AND FILTER COMMANDS
    case 'b':  // stream data
      if (SDfileOpen) stampSD(ACTIVATE);                    // time stamp the start time
      if (OBCI.useAccel) {
        OBCI.enable_accel(RATE_25HZ); // fire up the accelerometer if you want it
      }
      startRunning(outputType);                             // turn on the fire hose
      break;
    case 's':  // stop streaming data
      if (SDfileOpen) stampSD(DEACTIVATE);      // time stamp the stop time
      if (OBCI.useAccel) {
        OBCI.disable_accel(); // shut down the accelerometer if you're using it
      }
      stopRunning();
      break;
    case 'f':
      useFilters = true;
      break;
    case 'g':
      useFilters = false;
      break;

    //  INITIALIZE AND VERIFY
    case 'v':
      startFromScratch();  // initialize ADS and read device IDs
      break;
    //  QUERY THE ADS AND ACCEL REGITSTERS
    case '?':
      printRegisters();     // print the ADS and accelerometer register values
      break;
    default:
      break;
  }
}// end of getCommand

void sendEOT() {
  Serial0.print("$$$");  // shake hands with the controlling program
}

void loadChannelSettings(char c) {

  if (channelSettingsCounter == 0) { // if it's the first byte in this channel's array, this byte is the channel number to set
    currentChannelToSet = getChannelNumber(c); // we just got the channel to load settings into (shift number down for array usage)
    channelSettingsCounter++;
    if (!is_running) {
      Serial0.print("load setting ");
      Serial0.print("for channel ");
      Serial0.println(currentChannelToSet + 1, DEC);
    }
    return;
  }
  //  setting bytes are in order: POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET
  if (!is_running) {
    Serial0.print(channelSettingsCounter - 1);
    Serial0.print(" with "); Serial0.println(c);
  }
  c -= '0';
  if (channelSettingsCounter - 1 == GAIN_SET) {
    c <<= 4;
  }
  OBCI.channelSettings[currentChannelToSet][channelSettingsCounter - 1] = c;
  channelSettingsCounter++;
  if (channelSettingsCounter == 7) { // 1 currentChannelToSet, plus 6 channelSetting parameters
    if (!is_running) Serial0.print("done receiving settings for channel "); Serial0.println(currentChannelToSet + 1, DEC);
    getChannelSettings = false;
  }
}

void writeChannelSettings_maintainRunningState(char chan) {
  boolean is_running_when_called = is_running;
  int cur_outputType = outputType;
  stopRunning();                   //must stop running to change channel settings

  OBCI.writeChannelSettings(chan + 1);  // change the channel settings on ADS

  if (is_running_when_called == true) {
    startRunning(cur_outputType);  //restart, if it was running before
  }
}

void setChannelsToDefaultSetting() {
  boolean is_running_when_called = is_running;
  int cur_outputType = outputType;
  stopRunning();  //must stop running to change channel settings

  OBCI.setChannelsToDefault();   // default channel settings

  if (is_running_when_called == true) {
    startRunning(cur_outputType);  //restart, if it was running before
  }
}

void loadLeadOffSettings(char c) {
  if (leadOffSettingsCounter == 0) { // if it's the first byte in this channel's array, this byte is the channel number to set
    currentChannelToSet = getChannelNumber(c); // we just got the channel to load settings into (shift number down for array usage)
    if (!is_running) Serial0.print("changing LeadOff settings for channel "); Serial0.println(currentChannelToSet + 1, DEC);
    leadOffSettingsCounter++;
    return;
  }
  //  setting bytes are in order: PCHAN, NCHAN
  if (!is_running) {
    Serial0.print("load setting "); Serial0.print(leadOffSettingsCounter - 1);
    Serial0.print(" with "); Serial0.println(c);
  }
  c -= '0';
  OBCI.leadOffSettings[currentChannelToSet][leadOffSettingsCounter - 1] = c;
  leadOffSettingsCounter++;
  if (leadOffSettingsCounter == 3) { // 1 currentChannelToSet, plus 2 leadOff setting parameters
    if (!is_running) Serial0.print("done receiving leadOff settings for channel "); Serial0.println(currentChannelToSet + 1, DEC);
    getLeadOffSettings = false; // release the serial COM
  }
}

char getChannelNumber(char n) {
  if (n > '0' && n < '9') {
    n -= '1';
  }
  switch (n) {
    case 'Q':
      n = 0x08; break;
    case 'W':
      n = 0x09; break;
    case 'E':
      n = 0x0A; break;
    case 'R':
      n = 0x0B; break;
    case 'T':
      n = 0x0C; break;
    case 'Y':
      n = 0x0D; break;
    case 'U':
      n = 0x0E; break;
    case 'I':
      n = 0x0F; break;
    default: break;
  }
  return n;
}

void changeChannelState_maintainRunningState(byte chan, int start)
{
  boolean is_running_when_called = is_running;
  int cur_outputType = outputType;

  //must stop running to change channel settings
  stopRunning();
  if (start == 1) {
    OBCI.activateChannel(chan);
  } else if (start == 0) {
    OBCI.deactivateChannel(chan);
  }
  //restart, if it was running before
  if (is_running_when_called == true) {
    startRunning(cur_outputType);
  }
}

void activateAllChannelsToTestCondition(byte testInputCode, byte amplitudeCode, byte freqCode)
{
  boolean is_running_when_called = is_running;
  int cur_outputType = outputType;
  //must stop running to change channel settings
  stopRunning(); delay(10);

  //set the test signal to the desired state
  OBCI.configureInternalTestSignal(amplitudeCode, freqCode);
  //change input type settings for all channels
  OBCI.changeInputType(testInputCode);

  //restart, if it was running before
  if (is_running_when_called == true) {
    startRunning(cur_outputType);
  }
}

int changeChannelLeadOffDetect_maintainRunningState(char chan)
{
  boolean is_running_when_called = is_running;
  int cur_outputType = outputType;

  //must stop running to change channel settings
  stopRunning();

  OBCI.changeChannelLeadOffDetect(chan);

  //restart, if it was running before
  if (is_running_when_called == true) {
    startRunning(cur_outputType);
  }
}

void sendDefaultChannelSettings() {
  boolean is_running_when_called = is_running;
  int cur_outputType = outputType;

  OBCI.reportDefaultChannelSettings();
  sendEOT();
  delay(10);

  //restart, if it was running before
  if (is_running_when_called == true) {
    startRunning(cur_outputType);
  }
}

boolean stopRunning(void) {
  if (is_running) {
    OBCI.stopStreaming();  // stop the data acquisition, turn off accelerometer
    is_running = false;
  }
  return is_running;
}

boolean startRunning(int OUT_TYPE) {
  if (!is_running) {
    outputType = OUT_TYPE;
    OBCI.startStreaming();  // start the data acquisition, turn on accelerometer
    is_running = true;
  }
  return is_running;
}

void printRegisters() {

  if (!is_running) {
    // print the ADS and LIS3DH registers
    OBCI.printAllRegisters();
    sendEOT();
    delay(20);
  }

}

void startFromScratch() {
  if (!is_running) {
    OBCI.initialize();     // initializes accelerometer and on-board ADS and on-daisy ADS if present
    delay(500);
    Serial0.println("OpenBCI V3 16 channel");
    OBCI.configureLeadOffDetection(LOFF_MAG_6NA, LOFF_FREQ_31p2HZ);
    Serial0.print("On Board ADS1299 Device ID: 0x"); Serial0.println(OBCI.ADS_getDeviceID(ON_BOARD), HEX);
    if (OBCI.daisyPresent) { // library will set this in initialize() if daisy present and functional
      Serial0.print("On Daisy ADS1299 Device ID: 0x"); Serial0.println(OBCI.ADS_getDeviceID(ON_DAISY), HEX);
    }
    Serial0.print("LIS3DH Device ID: 0x"); Serial0.println(OBCI.LIS3DH_getDeviceID(), HEX);
    sendEOT();
  }
}



int main() 
{
  setup();

  while (1) loop();
  /*
    int i = 0;
    int data;
    adc_SS = 1;
    
    adc_reset = 0;
    //uart.printf("\nStart!\n");
    adc_reset = 1;  
  
  
  
    

  
    
    while (1) {
        led_blue = !led_blue;
        wait(1.0);   
        //uart.printf("main %d\r\n", i++);
      
      
      
      //adc_SS = 0;
      //spi_adc.write(0x20 + 0);
      //spi_adc.write(0);
      //data = spi_adc.write(0);
      //adc_SS = 1;
      
      
    adc_SS = 0;
    
    //spi_adc.write(0x40 + 0x01); // CONFIG1
    //spi_adc.write(0x00);           //  Send number of registers to read -1
    //spi_adc.write(0xB6);         //  Write the value to the register
    adc_SS = 1;       //  close SPI
      
      //uart.printf("adc %x\r\n", data);
    }
*/
}

