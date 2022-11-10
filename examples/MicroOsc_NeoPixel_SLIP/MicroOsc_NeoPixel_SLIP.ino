// MicroOsc_NeoPixel_SLIP
// by Thomas O Fredericks
// 2022-11-09

// HARDWARE REQUIREMENTS
// ==================
// NeoPixel LED strip connected to pin 4

// REQUIRED LIBRARIES
// ==================
// MicroOsc
// Adafruit NeoPixel

// REQUIRED CONFIGURATION
// ======================
// Set Serial baud to 115200


#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// If you want to control 30 RGB NeoPixels, you need at least 90 bytes for the data (1 byte per color). We are reserving a little more (128 bytes) just in case.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myMicroOsc(&Serial);

#include <Adafruit_NeoPixel.h>  
const int myPixelCount = 30;
const int myPixelPin = 4;
Adafruit_NeoPixel myPixelStrip(myPixelCount, myPixelPin , NEO_RGB + NEO_KHZ800);

void setup() {
  // INITIATE SERIAL COMMUNICATION
  Serial.begin(115200);                                         
  // INITIALIZE PIXEL STRIP
  myPixelStrip.begin(); 
  // STARTUP ANIMATION
  // LIGHT ALL PIXELS IN WHITE THEN TURN THEM ALL OFF
  for ( int i=0; i < myPixelCount; i++ ) {
     myPixelStrip.setPixelColor(i, myPixelStrip.Color(255, 255 , 255));  
     myPixelStrip.show();
     delay(100);                                       
  }
  delay(1000);
  myPixelStrip.clear();
  myPixelStrip.show();
}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED OSC MESSAGE
void myOnReceiveMessage( MicroOscMessage& receivedOscMessage ) {
  // IF THE ADDRESS IS /rgb
  if ( receivedOscMessage.fullMatch("/rgb") ) {   
                 
    // CREATE A VARIABLE TO STORE THE POINTER TO THE DATA
    const uint8_t* blobData;                                    
    // GET THE DATA SIZE AND THE POINTER TO THE DATA
    int blobSize = receivedOscMessage.nextAsBlob(&blobData);    
    // IF DATA SIZE IS LARGER THAN 0 AND IS A MULTIPLE OF 3 AS REQUIRED BY RGB PIXELS
    if ( blobSize > 0 && blobSize % 3 == 0 ) {                  
      // DIVIDE THE DATA BY 3 TO GET THE NUMBER OF PIXELS
      int blobPixelCount = blobSize / 3;                        
      // ITERATE THROUGH EACH PIXEL IN THE BLOB 
      for ( int i = 0 ; i < blobPixelCount ; i++ ) {            
        // EACH PIXEL HAS 3 BYTES, SO WE GO THROUGH THE DATA, 3 AT A TIME
        int blobIndex = i * 3; 
        uint8_t red =  myPixelStrip.gamma8(blobData[blobIndex]);
        uint8_t green =  myPixelStrip.gamma8(blobData[blobIndex+1]); 
        uint8_t blue =  myPixelStrip.gamma8(blobData[blobIndex+2]);                            
        myPixelStrip.setPixelColor(i, myPixelStrip.Color(red , green , blue));
      }
      myPixelStrip.show();
    }
  }
}

void loop() {
  myMicroOsc.receiveMessages( myOnReceiveMessage );
}