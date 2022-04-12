// HARDWARE REQUIREMENTS
// ==================
// Example for ESP32

// REQUIRED LIBRARIES
// ==================
// WiFiConnect (lite) library and required WifiClient and HTTPClient
// https://github.com/mrfaptastic/WiFiConnectLite


// REQUIRED CONFIGURATION
// ======================
// Change sendPort to target port
// Change sendIp to ip of target device

// Load Wi-Fi library
#include <WiFi.h>


#include "WiFiConnect.h"
WiFiConnect wifiConnect;

#include <WiFiUdp.h>
WiFiUDP udp;

unsigned int receivePort = 8888;

IPAddress broadcastIp(255, 255, 255, 255);
IPAddress sendIp(192, 168, 1, 225);
unsigned int sendPort = 7777;


#include <MicroOscUdp.h>
// The number 1024 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscUdp<1024> oscUdp(&udp, sendIp, sendPort);



/* We could also use button etc. to trigger the portal on demand within main loop */
void startWiFi() {

  //wifiConnect.resetSettings(); //helper to remove the stored wifi connection, comment out after first upload and re upload

  // Wifi Dies? Start Portal Again
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("DEBUG Trying to autoconnect...");
    // try to connect to wifi
    if (wifiConnect.autoConnect()) {
      Serial.println("DEBUG Autoconnected successfully!");
    } else {
      Serial.println("DEBUG Could not autoconnect. Starting configuration portal");
      /*
        AP_NONE = Continue executing code
        AP_LOOP = Trap in a continuous loop - Device is useless
        AP_RESET = Restart the chip
        AP_WAIT  = Trap in a continuous loop with captive portal until we have a working WiFi connection
      */
      wifiConnect.startConfigurationPortal(AP_WAIT);//if not connected show the configuration portal
    }
  } else {
    Serial.println("DEBUG WiFi already connected!");
  }

}

void setup() {

  // INITIATE SERIAL COMMUNICATION
   Serial.begin(115200);
  delay(1000);
  Serial.println("DEBUG Starting WiFi");
  startWiFi();


  udp.begin(receivePort);


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

    oscUdp.sendMessage("/test/i",  "i",   (uint32_t) millis());
    delay(1000);

    oscUdp.sendMessage("/test/f",  "f",   ((float)millis()) *0.001);
    delay(1000);

    uint8_t blob[3];
    blob[0] = millis() % 256;
    blob[1] = (millis()+1) % 256;
    blob[2] = (millis()+2) % 256;
    oscUdp.sendMessage("/test/b",  "b",   3 , blob);
    delay(1000);

    const char * hello = "hello";
    oscUdp.sendMessage("/test/s",  "s",  hello);
    delay(1000);

   uint8_t midi[4];
    midi[0] = millis() % 128;
    midi[1] = (millis()+1) % 128;
    midi[2] = (millis()+2) % 128;
    midi[3] = (millis()+3) % 128;
    oscUdp.sendMessage("/test/m",  "m",  midi);
    delay(1000);


    oscUdp.sendMessage("/test/t",  "t",  (uint64_t)(millis()));
    delay(1000);
    
   
}
