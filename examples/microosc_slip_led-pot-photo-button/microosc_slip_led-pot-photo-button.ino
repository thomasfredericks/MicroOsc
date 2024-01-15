// MicroOsc_Demo_SLIP
// by Thomas O Fredericks
// 2023-02-20

// WHAT IS DOES
// ======================
// OSC communication example for the Arduino Nano.
//
// OSC messages received by the Arduino :
// - /led int -> turn off (0) or on (1) a LED 
// - /pwm int -> set the pwm (0-255) of the arcade button LED 
//
// OSC messages sent by the Arduino :
// - /pot int -> sends the value of a potentiometer 
// - /photo int -> sends the value of a photocell
// - /button int -> sends the value of a button

// HARDWARE REQUIREMENTS
// ==================
// - POTENTIOMETER connected to analog pin A0
// - POTOCELL (with voltage resistor) connected to analog pin A1
// - Illuminated arcade BUTTON with it's switch connected to pin 2
//   and it's LED connected to pin 3 (will be PWM modulated)
// - LED (and matching resistor) connected to pin 5 (will be turned off or on)

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
int myPotPin = 0;

// PHOTOCELL
int myPhotoPin = 1;

// BUTTON
int myButtonPin = 2;

// BUTON LED
int myButtonLedPin = 3;  // PIN MUST SUPPORT PWM

// LED
int myLedPin = 5;

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

  // Gives time for the USB drivers to settle
  delay(5000);
}

/****************
  ON OSC MESSAGE
*****************/
void myOnOscMessageReceived(MicroOscMessage& oscMessage) {
  
  if (oscMessage.checkOscAddress("/led")) {  // IF THE ADDRESS IS /led
    int newValue = oscMessage.nextAsInt();  // GET NEW VALUE AS INT
    digitalWrite(myLedPin, newValue);       // SET LED OUTPUT TO VALUE (DIGITAL: OFF/ON)
    
  } else if (oscMessage.checkOscAddress("/buttonLed")) {  // IF THE ADDRESS IS /buttonLed
    int newValue = oscMessage.nextAsInt();  // GET NEW VALUE AS INT
    analogWrite(myButtonLedPin, newValue);       // SET LED OUTPUT TO VALUE (ANALOG/PWM: 0-255)

  }
  
}


/*******
  LOOP
********/
void loop() {

  myMicroOsc.onOscMessageReceived(myOnOscMessageReceived);  // TRIGGER OSC RECEPTION

  if (millis() - myChronoStart >= 50) {  // IF 50 MS HAVE ELLAPSED
    myChronoStart = millis();            // RESTART CHRONO

    myMicroOsc.sendInt("/pot", analogRead(myPotPin) );  // SEND POTENTIOMETER

    myMicroOsc.sendInt("/photo", analogRead(myPhotoPin) );  // SEND PHOTOCELL

    myMicroOsc.sendInt("/button", digitalRead(myButtonPin));  // SEND BUTTON
  }
}
