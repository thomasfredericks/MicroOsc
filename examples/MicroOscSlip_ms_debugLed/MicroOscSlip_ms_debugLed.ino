// DESCRIPTION
// ==================
// - When OSC message "/debugLed" is received, it turns on or off the debug LED depending on the uint32_t value.
// - Sends OSC message "/ms" followed by the uint32_t value of millis() every second.

// HARDWARE REQUIREMENTS
// ==================
// - A board with a debug LED.

// REQUIRED LIBRARIES
// ==================
// - MicroOsc

// REQUIRED CONFIGURATION
// ======================
// - Make sure serial baud is 38400.
// - If your board does not have a debug LED, select a pin and and one (with a current limiting resistor).

const int debugLedPin = 13;

unsigned long lastTimeMessageWasSent;

#include <MicroOscSlip.h>
// The number 32 between the < > below is the number of bytes allocated for incomming messages.
// Every incomming OSC message must be able to fit in this buffer.
// Outgoing messages are written directly to the output stream and do not require additionnal allocated bytes.
MicroOscSlip<32> osc(&Serial);


void setup() {
  // INITIATE SERIAL COMMUNICATION.
  Serial.begin(38400);
  delay(1000);

  pinMode(debugLedPin , OUTPUT);
}

// FUNCTION THAT IS SET TO BE CALLED WHEN A MESSAGE IS RECEIVED.
void receivedOscMessage( MicroOscMessage & message) {
  if ( message.fullMatch("/debugLed", "i") ) {
    int32_t firstArgument = message.getNextInt32();
    digitalWrite(debugLedPin, firstArgument);
  }
}

void loop() {
  // CALL receivedOscMessage WHEN A MESSAGE IS RECEIVED.
  osc.receiveMessages( receivedOscMessage );

  unsigned long now = millis();
  if ( now - lastTimeMessageWasSent > 1000 ) {
    lastTimeMessageWasSent = now;
    osc.sendMessage("/ms",  "i",  (int32_t) millis());
  }

}
