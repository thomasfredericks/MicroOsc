/*
  MicroOsc SLIP example for multiple arguments.
  By Thomas O Fredericks.
  2024-01-14

  WHAT IS DOES
  ======================
  Example and test code for how to send and receive multiple argument OSC messages with MicroOsc.

  HARDWARE REQUIREMENTS
  ==================
  - Any "regular" Arduino.


  REQUIRED LIBRARIES
  ==================
  - MicroOsc.

  REQUIRED CONFIGURATION
  ======================
  - Set the baud of the application receiving the OSC SLIP messages to 115200.

*/

#include <MicroOscSlip.h>

// THE NUMBER 64 BETWEEN THE < > SYMBOLS  BELOW IS THE MAXIMUM NUMBER OF BYTES RESERVED FOR INCOMMING MESSAGES.
// MAKE SURE THIS NUMBER OF BYTES CAN HOLD THE SIZE OF THE MESSAGE YOUR ARE RECEIVING IN ARDUINO.
// OUTGOING MESSAGES ARE WRITTEN DIRECTLY TO THE OUTPUT AND DO NOT NEED ANY RESERVED BYTES.
MicroOscSlip<64> myMicroOsc(&Serial);  // CREATE AN INSTANCE OF MicroOsc FOR SLIP MESSAGES

/********
  SETUP
*********/
void setup() {

  Serial.begin(115200);
}

/****************
  myOnOscMessageReceived is triggered when a message is received
*****************/
void myOnOscMessageReceived(MicroOscMessage& oscMessage) {

  // CHECK THE ADDRESS OF THE OSC MESSAGE
  if (oscMessage.checkOscAddress("/echo", "sfi")) {

    char stringArgument[20];
    char* stringPointer = oscMessage.nextAsString();
    size_t copiedChars = snprintf(stringArgument, 20, "%s", stringPointer);

    float floatArgument = oscMessage.nextAsFloat();

    int intArgument = oscMessage.nextAsInt();

    myMicroOsc.sendMessage("/echo", "sfi", stringArgument, floatArgument, intArgument);

  } else if (oscMessage.checkOscAddress("/echo", "fsi")) {

    float floatArgument = oscMessage.nextAsFloat();

    char stringArgument[20];
    char* stringPointer = oscMessage.nextAsString();
    size_t copiedChars = snprintf(stringArgument, 20, "%s", stringPointer);

    int intArgument = oscMessage.nextAsInt();

    myMicroOsc.sendMessage("/echo", "fsi", floatArgument, stringArgument, intArgument);
  }
}

/*******
  LOOP
********/
void loop() {

  // TRIGGER myOnOscMessageReceived() IF AN OSC MESSAGE IS RECEIVED :
  myMicroOsc.onOscMessageReceived(myOnOscMessageReceived);

  // SEND OSC MESSAGES EVERY 20 MILLISECONDS :
  static unsigned long myChronoStart;
  if (millis() - myChronoStart >= 10) {  // IF XX ms HAVE ELLAPSED
    myChronoStart = millis();            // RESTART CHRONO

    float f_millis = float(millis()) / 1000.0;

    char s_millis[20];
    size_t writtenChars = snprintf(s_millis, 20, "milliseconds:%ld", millis());

    // void sendMessage(const char *address, const char *format, ... );
    myMicroOsc.sendMessage("/stuff", "fsi", f_millis, s_millis, millis());

    // USE THE FOLLOWING METHODS TO SEND OSC MESSAGES :
    /*
      // SEND AN INT(32)
      myMicroOsc.sendInt(const char *address, int32_t i);
      // SEND A FLOAT
      myMicroOsc.sendFloat(const char *address, float f);
      // SEND A STRING
      myMicroOsc.endString(const char *address, const char *str);
      // SEND A BLOB
      myMicroOsc.sendBlob(const char *address, unsigned char *b, int32_t length);
      // SEND DOUBLE
      myMicroOsc.sendDouble(const char *address,double d);
      // SEND MIDI
      myMicroOsc.sendMidi(const char *address,unsigned char *midi);
      // SEND INT64
      myMicroOsc.sendInt64(const char *address, uint64_t h);
      // SEND A MIXED TYPE VARIABLE LENGTH MESSAGE
      myMicroOsc.sendMessage(const char *address, const char *format, ...);
    */
  }
}