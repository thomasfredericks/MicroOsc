/*
  MicroOSC minimal SLIP code for the M5Stack Atom Lite.
  By Thomas O Fredericks.
  2023-09-20

  WHAT IS DOES
  ======================
  Basic Framework for OSC communication.

  HARDWARE REQUIREMENTS
  ==================
  - M5Stack Atom Lite.


  REQUIRED LIBRARIES
  ==================
  - MicroOsc.
  - M5Atom.

  REQUIRED CONFIGURATION
  ======================
  - Set the baud of the application receiving the OSC SLIP messages to 115200.

*/

#include <M5Atom.h>
#include <FastLED.h>

CRGB mesPixels[1];

#include <MicroOscSlip.h>

// THE NUMBER 64 BETWEEN THE < > SYMBOLS  BELOW IS THE MAXIMUM NUMBER OF BYTES RESERVED FOR INCOMMING MESSAGES.
// MAKE SURE THIS NUMBER OF BYTES CAN HOLD THE SIZE OF THE MESSAGE YOUR ARE RECEIVING IN ARDUINO.
// OUTGOING MESSAGES ARE WRITTEN DIRECTLY TO THE OUTPUT AND DO NOT NEED ANY RESERVED BYTES.
MicroOscSlip<64> myMicroOsc(&Serial);  // CREATE AN INSTANCE OF MicroOsc FOR SLIP MESSAGES

unsigned long myChronoStart = 0;  // VARIABLE USED TO LIMIT THE SPEED OF THE SENDING OF OSC MESSAGES



/********
  SETUP
*********/
void setup() {

  M5.begin(false, false, false);

  FastLED.addLeds<WS2812, DATA_PIN, GRB>(mesPixels, 1);

  Serial.begin(115200);

  // Start-up animation
  // Gives time for the USB drivers to settle
  while (millis() < 5000) {
    mesPixels[0] = CHSV((millis() / 5) % 255, 255, 255 - (millis() * 255 / 5000));
    FastLED.show();
    delay(50);
  }
  mesPixels[0] = CRGB(0, 0, 0);
  FastLED.show();
}

/****************
  myOnOscMessageReceived is triggered when a message is received
*****************/
void myOnOscMessageReceived(MicroOscMessage& oscMessage) {

  // CHECK THE ADDRESS OF THE OSC MESSAGE
  if (oscMessage.checkOscAddress("/pixel")) {

    int red = oscMessage.nextAsInt();
    int green = oscMessage.nextAsInt();
    int blue = oscMessage.nextAsInt();
    mesPixels[0] = CRGB(red, green, blue);
    FastLED.show();

  } else if (oscMessage.checkOscAddress("/address")) {

    // USE THE FOLLOWING METHODS TO PARSE INDIVIDUAL ARGUMENTS :
    /*
      // PARSE AN INT
      int32_t intArgument = receivedOscMessage.nextAsInt();
      // PARSE AN FLOAT
      float floatArgument = receivedOscMessage.nextAsFloat();
      // PARSE AN STRING
      const char * s = receivedOscMessage.nextAsString();
      // PARSE A BLOB
      const uint8_t* blob;
      uint32_t length = receivedOscMessage.nextAsBlob(&blob);
      // PARSE MIDI
      const uint8_t* midi;
      receivedOscMessage.nextAsMidi(&midi);
    */
  }
}

/*******
  LOOP
********/
void loop() {

  M5.update();

  // TRIGGER myOnOscMessageReceived() IF AN OSC MESSAGE IS RECEIVED :
  myMicroOsc.onOscMessageReceived(myOnOscMessageReceived);

  // SEND OSC MESSAGES (EVERY 50 MILLISECONDS) :
  if (millis() - myChronoStart >= 50) {  // IF 50 MS HAVE ELLAPSED
    myChronoStart = millis();            // RESTART CHRONO

    myMicroOsc.sendInt("/millis", millis());

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