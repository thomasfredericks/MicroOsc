/*
  MicroOsc SLIP example for multiple arguments.
  By Thomas O Fredericks.
  2024-01-14

  WHAT IS DOES
  ======================
  Example and test code for how to copy the address and type tags for advanced parsing.

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

  char address[32];
  // COPY THE ADDRESS
  oscMessage.copyAddress(address, 32);

  char typeTags[8];
  // COPY THE TYPE TAGS
  oscMessage.copyTypeTags(typeTags, 8);

  // Using strtok to tokenize the string
  /*
  Keep in mind that strtok modifies the input string by replacing the delimiter with null characters. 
  If you need to keep the original string intact, you may want to make a copy of it before using strtok.
  Also, note that strtok is not thread-safe. If you are working in a multithreaded environment, 
  consider using strtok_r (where available) or other thread-safe alternatives.
  */
  char* token = strtok(address, "/");
  int tokenIndex = 0;

  // Loop through the tokens
  while (token != NULL) {
    myMicroOsc.sendString("/token", token);

    // Get the next token
    token = strtok(NULL, "/");
    tokenIndex++;
  }

  myMicroOsc.sendString("/typetags", typeTags);

}

/*******
  LOOP
********/
void loop() {

  // TRIGGER myOnOscMessageReceived() IF AN OSC MESSAGE IS RECEIVED :
  myMicroOsc.onOscMessageReceived(myOnOscMessageReceived);
}