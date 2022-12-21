// MicroOsc_Demo_SLIP
// by Thomas O Fredericks
// 2022-12-21

// HARDWARE REQUIREMENTS
// ==================
// - Potentiometer connected to analog pin A0
// - Photocell connected to analog pin A1
// - Switch connected to pin 2
// - LED connected to pin 3

// REQUIRED LIBRARIES
// ==================
// - MicroOsc

// REQUIRED CONFIGURATION
// ======================
// - Set the baud of your computer's serial connection to 115200

#include <MicroOsc.h>
#include <MicroOscUdp.h>
#include <MicroOscSlip.h>

// The number 64 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<64> myMicroOsc(&Serial);

unsigned long myChronoStart = 0;

// POTENTIOMETER
int  myPotPin = A0;
int  myPotStoredValue = 0;

// PHOTOCELL
int myPhotoPin = A1;
int myPhotoStoredValue = 0;

// BUTTON
int myButtonPin = 2;
int myButtonSotredValue = 0;

// LED
int myLedPin = 3;

/********
  SETUP
*********/
void setup() {
  Serial.begin(115200); // START SERIAL COMMUNICATION
  pinMode( myPotPin , INPUT);  // POTENTIOMETER: ANALOG INPUT
  pinMode( myPhotoPin , INPUT);  // PHOTOCELL: ANALOG INPUT
  pinMode( myButtonPin , INPUT_PULLUP);  // BUTTON: DIGITAL INPUT
  pinMode( myLedPin , OUTPUT); // LED: DIGITAL OUTPUT
}

/****************
  ON OSC MESSAGE
*****************/
void myOnOscMessageReceived( MicroOscMessage& oscMessage ) { 
  if ( oscMessage.checkOscAddress("/led") ) { // IF THE ADDRESS IS /led
    int newValue = oscMessage.nextAsInt(); // GET NEW VALUE AS INT
    digitalWrite( myLedPin , newValue); // SET LED OUTPUT TO VALUE
  }
}

/****************
  POTENTIOMETER UPDATE
****************/
void myPotUpdateValueAndSendIfChanged() {
  int newValue = analogRead(myPotPin); // READ NEW VALUE
  if ( newValue != myPotStoredValue ) { // IF NEW VALUE DIFFERENT THAN STORED VALUE
    myPotStoredValue = newValue; // STORE NEW VALUE
    myMicroOsc.sendInt( "/pot" , myPotStoredValue  ); // SEND UPDATED VALUE
  }
}

/*********************
  PHOTOCELL UPDATE
**********************/
void myPhotoUpdateValueAndSendIfChanged() {
    int newValue = analogRead(myPhotoPin); // READ CURRENT VALUE
    if ( newValue != myPhotoStoredValue) { // IF NEW VALUE DIFFERENT THAN STORED VALUE
      myPhotoStoredValue = newValue; // STORE NEW VALUE
      myMicroOsc.sendInt( "/photo"  , myPhotoStoredValue ); // SEND UPDATED VALUE

    }  
}

/*********************
  BUTTON UPDATE
**********************/
void myButtonUpdateValueAndSendIfChanged() {
    int newValue = digitalRead(myButtonPin); // READ CURRENT VALUE
    if ( newValue != myButtonSotredValue ) { // IF NEW VALUE DIFFERENT THAN STORED VALUE
      myButtonSotredValue = newValue; // STORE NEW VALUE
      myMicroOsc.sendInt( "/button"  , myButtonSotredValue  ); // SEND UPDATED VALUE
    }  
}

/*******
  LOOP
********/
void loop() {

  myMicroOsc.onOscMessageReceived( myOnOscMessageReceived ); // TRIGGER OSC RECEPTION

  if ( millis() - myChronoStart >= 50) { // IF 50 MS HAVE ELLAPSED
    myChronoStart = millis(); // RESTART CHRONO

    myPotUpdateValueAndSendIfChanged(); // POTENTIOMETER UPDATE

    myPhotoUpdateValueAndSendIfChanged(); // PHOTOCELL UPDATE

    myButtonUpdateValueAndSendIfChanged(); // BUTTON UPDATE

  }
}
