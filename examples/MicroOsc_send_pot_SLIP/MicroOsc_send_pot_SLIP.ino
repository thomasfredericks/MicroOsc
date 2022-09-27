// HARDWARE REQUIREMENTS
// ==================
// Potentiometer connected to analog pin A0

// REQUIRED LIBRARIES
// ==================
// MicroOsc

// REQUIRED CONFIGURATION
// ======================
// Set Serial baud to 115200


#include <MicroOscSlip.h>
// The number 64 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<64> myMicroOsc(&Serial);

unsigned long myChronoStart = 0;

const int myPotentiometerPin = A0;
int myPreviousPotValue;

void setup() {
  // INITIATE SERIAL COMMUNICATION
  Serial.begin(115200);
  pinMode( myPotentiometerPin , INPUT);
}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void myReceiveOscMessage( MicroOscMessage & myOscMessage ) {
  // THIS IS NOT USED
}

void loop() {
  myMicroOsc.receiveMessages( myReceiveOscMessage );

  unsigned long myChronoElapsed = millis() - myChronoStart;
  unsigned long myChronoInterval = 50;
  if ( myChronoElapsed >= myChronoInterval) {
    myChronoStart = millis();

    
    int myPotValue = analogRead(myPotentiometerPin);

    if ( myPotValue != myPreviousPotValue ) {
      myMicroOsc.sendInt( "/pot" ,myPotValue  );
    }
    myPreviousPotValue = myPotValue;
    
  }
  
  
}
