// HARDWARE REQUIREMENTS
// ==================
// None

// REQUIRED LIBRARIES
// ==================
// None

// REQUIRED CONFIGURATION
// ======================
// Set Serial baud for monitoring to 115200


#include <MicroOscSlip.h>
// The number 64 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<64> osc(&Serial);

void setup() {
  // INITIATE SERIAL COMMUNICATION
  Serial.begin(115200);
}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage( MicroOscMessage & message) {
  // WHEN A MESSAGE IS MATCHED IT ECHOS IT THROUGH SERIAL SLIP
  if ( message.fullMatch("/test/i", "i") ) {
    int32_t firstArgument = message.getNextInt32();
    osc.sendMessage( "/test/i",  "i",  firstArgument);

  } else if ( message.fullMatch("/test/f",  "f")) {
    float firstArgument = message.getNextFloat();
    osc.sendMessage( "/test/f",  "f",  firstArgument);

  } else if ( message.fullMatch("/test/b",  "b")) {
    const uint8_t* blob;
    size_t length;
    message.getNextBlob(&blob, &length);
    if ( blob != NULL && length != 0) {
      osc.sendMessage( "/test/b", "b", length, blob);
    }

  } else if ( message.fullMatch("/test/s",  "s")) {
    const char * s = message.getNextString();
    osc.sendMessage( "/test/s",  "s",  s);

  } else if ( message.fullMatch("/test/m", "m")) {
    const uint8_t* midi;
    message.getNextMidi(&midi);
    if ( midi != NULL ) {
      osc.sendMessage( "/test/m",  "m", midi);
    }
  }
}

void loop() {

  osc.receiveMessages( receivedOscMessage );

}
