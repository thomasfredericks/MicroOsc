// MicroOsc_Demo_SLIP
// by Thomas O Fredericks
// 2022-12-22

// HARDWARE REQUIREMENTS
// ==================
// - POTENTIOMETER connected to analog pin A0
// - POTOCELL (with voltage resistor) connected to analog pin A1
// - Illuminated arcade BUTTON with it's switch connected to pin 2
//   and it's LED connected to pin 3
// - LED (and matching resistor) connected to pin 5

// REQUIRED LIBRARIES
// ==================
// - MicroOsc

// REQUIRED CONFIGURATION
// ======================
// - Set the baud of your computer's serial connection to 115200

#include <MicroOscSlip.h>

// THE NUMBER 64 BETWEEN THE < > SYMBOLS  BELOW IS THE MAXIMUM NUMBER OF BYTES RESERVED FOR INCOMMING MESSAGES.
// MAKE SURE THIS NUMBER OF BYTES CAN HOLD THE SIZE OF THE MESSAGE YOUR ARE RECEIVING IN ARDUINO.
// OUTGOING MESSAGES ARE WRITTEN DIRECTLY TO THE OUTPUT AND DO NOT NEED ANY RESERVED BYTES.
MicroOscSlip<64> myMicroOsc(&Serial);  // CREATE AN INSTANCE OF MicroOsc FOR SLIP MESSAGES

unsigned long myChronoStart = 0;  // VARIABLE USED TO LIMIT THE SPEED OF THE loop() FUNCTION.

// POTENTIOMETER
int myPotPin = A0;
int myPotStoredValue = 0;

// PHOTOCELL
int myPhotoPin = A1;
int myPhotoStoredValue = 0;

// BUTTON
int myButtonPin = 2;
int myButtonSotredValue = 0;

// BUTON LED
int myButtonLedPin = 3;  // PIN MUST SUPPORT PWM

// LED
int myLedPin = 5;  // PIN MUST SUPPORT PWM

/********
  SETUP
*********/
void setup() {
  Serial.begin(115200);                // START SERIAL COMMUNICATION
  pinMode(myPotPin, INPUT);            // POTENTIOMETER: ANALOG INPUT
  pinMode(myPhotoPin, INPUT);          // PHOTOCELL: ANALOG INPUT
  pinMode(myButtonPin, INPUT_PULLUP);  // BUTTON: DIGITAL INPUT
  pinMode(myButtonLedPin, OUTPUT);     // LED: DIGITAL OUTPUT
  pinMode(myLedPin, OUTPUT);           // LED: DIGITAL OUTPUT
}

/****************
  ON OSC MESSAGE
*****************/
void myOnOscMessageReceived(MicroOscMessage& oscMessage) {
  if (oscMessage.checkOscAddress("/led")) {  // IF THE ADDRESS IS /led
    int newValue = oscMessage.nextAsInt();  // GET NEW VALUE AS INT
    analogWrite(myLedPin, newValue);       // SET LED OUTPUT TO VALUE (ANALOG/PWM: 0-255)

  } else if (oscMessage.checkOscAddress("/buttonLed")) {  // IF THE ADDRESS IS /buttonLed
    int newValue = oscMessage.nextAsInt();  // GET NEW VALUE AS INT
    digitalWrite(myLedPin, newValue);       // SET LED OUTPUT TO VALUE (DIGITAL: OFF/ON)
  }
}

/****************
  POTENTIOMETER UPDATE
****************/
void myPotUpdateValueAndSendIfChanged() {
  int newValue = analogRead(myPotPin);             // READ NEW VALUE
  if (newValue != myPotStoredValue) {              // IF NEW VALUE DIFFERENT THAN STORED VALUE
    myPotStoredValue = newValue;                   // STORE NEW VALUE
    myMicroOsc.sendInt("/pot", myPotStoredValue);  // SEND UPDATED VALUE
  }
}

/*********************
  PHOTOCELL UPDATE
**********************/
void myPhotoUpdateValueAndSendIfChanged() {
  int newValue = analogRead(myPhotoPin);               // READ CURRENT VALUE
  if (newValue != myPhotoStoredValue) {                // IF NEW VALUE DIFFERENT THAN STORED VALUE
    myPhotoStoredValue = newValue;                     // STORE NEW VALUE
    myMicroOsc.sendInt("/photo", myPhotoStoredValue);  // SEND UPDATED VALUE
  }
}

/*********************
  BUTTON UPDATE
**********************/
void myButtonUpdateValueAndSendIfChanged() {
  int newValue = digitalRead(myButtonPin);               // READ CURRENT VALUE
  if (newValue != myButtonSotredValue) {                 // IF NEW VALUE DIFFERENT THAN STORED VALUE
    myButtonSotredValue = newValue;                      // STORE NEW VALUE
    myMicroOsc.sendInt("/button", myButtonSotredValue);  // SEND UPDATED VALUE
  }
}

/*******
  LOOP
********/
void loop() {

  myMicroOsc.onOscMessageReceived(myOnOscMessageReceived);  // TRIGGER OSC RECEPTION

  if (millis() - myChronoStart >= 50) {  // IF 50 MS HAVE ELLAPSED
    myChronoStart = millis();            // RESTART CHRONO

    myPotUpdateValueAndSendIfChanged();  // POTENTIOMETER UPDATE

    myPhotoUpdateValueAndSendIfChanged();  // PHOTOCELL UPDATE

    myButtonUpdateValueAndSendIfChanged();  // BUTTON UPDATE
  }
}
