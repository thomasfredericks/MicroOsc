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
    int32_t i = message.nextAsInt();
    osc.sendInt( "/test/i",  i);

  } else if ( message.fullMatch("/test/f",  "f")) {
    float f = message.nextAsFloat();
    osc.sendFloat( "/test/f",  f);

  } else if ( message.fullMatch("/test/b",  "b")) {
    const uint8_t* blob;
    uint32_t length = message.nextAsBlob(&blob);
    if ( length != 0) {
      osc.sendBlob( "/test/b", blob, length);
    }

  } else if ( message.fullMatch("/test/s",  "s")) {
    const char * s = message.nextAsString();
    osc.sendString( "/test/s",  s);

  } else if ( message.fullMatch("/test/m", "m")) {
    const uint8_t* midi;
    message.nextAsMidi(&midi);
    if ( midi != NULL ) {
      osc.sendMidi( "/test/m",  midi);
    }
  }
}

void loop() {

  osc.receiveMessages( receivedOscMessage );

}