/*
  MicroOsc SLIP example for multiple arguments.
  By Thomas O Fredericks.
  2024-02-29

  WHAT IS DOES
  ======================
  Example and test code for how to copy the address and type tags and use a decision tree for the address matching.

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
  // Note that all "/" are removed
  char* tokenized = strtok(address, "/");

  // Example messages comming in
  // /a/pot
  // /b/cha1/test

  // Work through the tokenized strings
  if (strcmp(tokenized, "a") == 0) {
    // Get the next tokenized string
    tokenized = strtok(NULL, "/");
    if (strcmp(tokenized, "pot") == 0) {
      // WE FOUND /a/pot
      // Do something with /a/pot
      // Exit once we are done with /a/pot by calling return
      return;
    }
  } else if (strcmp(tokenized, "b") == 0) {

    // Get the next tokenized string
    tokenized = strtok(NULL, "/");

    if (strcmp(tokenized, "cha1") == 0) {

      // Get the next tokenized string
      tokenized = strtok(NULL, "/");

      if (strcmp(tokenized, "test") == 0) {
        // WE FOUND /b/cha1/test
        // Do something with /b/cha1/test
        // Exit once we are done with /b/cha1/test by calling return
        return;
      } else if (strcmp(tokenized, "bird") == 0) {
        // WE FOUND /b/cha1/bird
        // Do something with /b/cha1/bird
        // Exit once we are done with /b/cha1/bird by calling return
        return;
      }
    }
  }
  printf("nothing found");
}

/*******
  LOOP
********/
void loop() {

  // TRIGGER myOnOscMessageReceived() IF AN OSC MESSAGE IS RECEIVED :
  myMicroOsc.onOscMessageReceived(myOnOscMessageReceived);
}