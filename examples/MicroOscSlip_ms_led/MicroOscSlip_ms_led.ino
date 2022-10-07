// DESCRIPTION
// ==================
// - Sends OSC message "/ms" followed by the uint32_t value of millis() every second.
//
// - When OSC message "/debugLed" is received, it turns on or off the debug LED depending on the uint32_t value.


// HARDWARE REQUIREMENTS
// ==================
// - A board with a debug LED.

// REQUIRED LIBRARIES
// ==================
// - MicroOsc

// REQUIRED CONFIGURATION
// ======================
// - Make sure serial baud is 115200.
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
  Serial.begin(115200);
  delay(1000);

  pinMode(debugLedPin , OUTPUT);
}

// FUNCTION THAT IS SET TO BE CALLED WHEN A MESSAGE IS RECEIVED.
void receivedOscMessage( MicroOscMessage & message) {
  if ( message.fullMatch("/debugLed", "i") ) {
    int32_t i = message.nextAsInt();
    if ( i == 0 ) digitalWrite(debugLedPin, LOW);
    else digitalWrite(debugLedPin, HIGH);
  }
}

void loop() {
  // CALL receivedOscMessage WHEN A MESSAGE IS RECEIVED.
  osc.receiveMessages( receivedOscMessage );

  unsigned long now = millis();
  if ( now - lastTimeMessageWasSent > 1000 ) {
    lastTimeMessageWasSent = now;
    osc.sendInt("/ms",  millis());
  }

}