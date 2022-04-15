// HARDWARE REQUIREMENTS
// ==================
// None

// REQUIRED LIBRARIES
// ==================
// None



// REQUIRED CONFIGURATION
// ======================
// Make sure serial baud is 38400


#include <MicroOscSlip.h>
// The number 32 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<32> osc(&Serial);


void setup() {

  // INITIATE SERIAL COMMUNICATION
  Serial.begin(38400);
  delay(1000);
}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE.
/*
  void receivedOscMessage( TinyOscMessage& message) {

  // WHEN THE "/test" MESSAGE THAT CONTAINS AN int, A float AND A string IS RECEIVED,
  // WE SEND AN "/echo" MESSAGE BACK WITH THE SAME ARGUMENTS BUT WITH THE NUMERICAL VALUES DOUBLED.

  if ( message.fullMatch("/test", "ifs") ) {
    int32_t firstArgument = message.getNextInt32();
    float secondArgument = message.getNextFloat();
    const char* thirdArgument = message.getNextString();

    firstArgument = firstArgument * 2;
    secondArgument = secondArgument * 2;

    oscUdp.sendMessage( "/echo",  "ifs",  firstArgument, secondArgument, thirdArgument );
    oscSlip.sendMessage( "/echo",  "ifs", firstArgument, secondArgument, thirdArgument );
  }

  }
*/


void loop() {

  osc.sendMessage("/test/i",  "i",   (uint32_t) millis());
  delay(1000);


}
