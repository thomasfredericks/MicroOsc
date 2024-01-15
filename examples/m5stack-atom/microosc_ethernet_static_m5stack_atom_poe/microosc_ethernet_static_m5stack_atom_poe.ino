/*
  MicroOSC minimal UDP Ethernet code for the M5Stack Atom Lite.
  By Thomas O Fredericks.
  2023-09-20

  WHAT IS DOES
  ======================
  Basic Framework for OSC communication.

  HARDWARE REQUIREMENTS
  ==================
  - M5Stack Atom POE.


  REQUIRED LIBRARIES
  ==================
  - MicroOsc.
  - M5Atom.
  - M5Ethernet.

  REQUIRED CONFIGURATION
  ======================
  - Set myDestinationIp below to the IP of the destination device.
  - Set myDestinationPort below to the listening port of the destination device.

  - Set myIp to the IP of this device.
  - Set myPort to the listening port of this device.


*/

#include <M5Atom.h>
#include <FastLED.h>

CRGB mesPixels[1];

#include <SPI.h>
#include <Ethernet.h>
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP myUdp;

IPAddress myDestinationIp(10, 1, 2, 3);
unsigned int myDestinationPort = 7001;

IPAddress myIp(10, 1, 2, 101);
unsigned int myPort = 7000;


#include <MicroOscUdp.h>

// THE NUMBER 1024 BETWEEN THE < > SYMBOLS  BELOW IS THE MAXIMUM NUMBER OF BYTES RESERVED FOR INCOMMING MESSAGES.
// OUTGOING MESSAGES ARE WRITTEN DIRECTLY TO THE OUTPUT AND DO NOT NEED ANY RESERVED BYTES.
// PROVIDE A POINTER TO UDP, AND THE IP AND PORT FOR OUTGOING MESSAGES.
// DO NOT FORGET THAT THE UDP CONNEXION MUST BE INITIALIZED IN SETUP() WITH THE RECEIVE PORT.
MicroOscUdp<1024> myMicroOsc(&myUdp, myDestinationIp, myDestinationPort);  // CREATE AN INSTANCE OF MicroOsc FOR UDP MESSAGES

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
  
  Serial.println();
  Serial.println("Starting Ethernet with STATIC IP");
  // CONFIGURE ETHERNET
  SPI.begin(22, 23, 33, 19);
  Ethernet.init(19);
  uint8_t myMac[6];
  // GET FACTORY DEFINED ESP32 MAC :
  esp_efuse_mac_get_default(myMac);
  // START ETHERNET WITH STATIC IP
  Ethernet.begin(myMac, myIp);

  myUdp.begin(myPort);

  Serial.println();
  Serial.println(__FILE__);
  Serial.print("myDestinationIp: ");
  Serial.println(myDestinationIp);
  Serial.print("myDestinationPort: ");
  Serial.println(myDestinationPort);
  Serial.print("myIp: ");
  Serial.println(Ethernet.localIP());
  Serial.print("myPort: ");
  Serial.println(myPort);
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
