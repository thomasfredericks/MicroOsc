# MicroOsc

MicroOsc is a simple and lightweight [Open Sound Control](http://opensoundcontrol.org/) (OSC) library for Arduino. 
MicroOsc provides a unified API to work over Serial (Slip) ad over UDP (Ethernet or Wifi). 
It does not hold on to state and error checking is minimal.
A few Arduino and application (Pure Data, Max, Node JS) examples are included.
MicroOsc was inspired by [TinyOSC](https://github.com/mhroth/TinyOSC) by Martin Roth.

## Supported Features
MicroOsc currently supports:
* Full address and format matching
* Message parsing
* Message writing
* Bundle parsing (as individual messages)
* Send Types
  * `b`: blob (byte array)
  * `f`: float
  * `d`: double
  * `i`: int (int32)
  * `h`: int64
  * `s`: string
  * `m`: midi

* Receive Types
  * `b`: blob (byte array)
  * `f`: float
  * `d`: double
  * `i`: int (int32)
  * `s`: string
  * `m`: midi

## Unsupported Features

MicroOsc will eventually but currently does *yet* not support:
* Send types not *yet* supported:
  * `t`: timetag
  * `T`: true
  * `F`: false
  * `I`: infinitum
  * `N`: nil
* Receive Types not *yet* supported:
  * `h`: int64
  * `t`: timetag
  * `T`: true
  * `F`: false
  * `I`: infinitum
  * `N`: nil
  
MicroOsc will probably never support:
* bundle writing (receiving is supported)
* timetags
* Regular expression matching

## Initialisation 

### OSC SLIP

```cpp
#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myOsc(&Serial);
```

### OSC UDP

Initilise UDP and network details first:
```cpp
#include <WiFiUdp.h>
WiFiUDP myUdp;
unsigned int myReceivePort = 8888;
IPAddress mySendIp(192, 168, 1, 210);
unsigned int mySendPort = 7777;
```

Initilise and include MicroOsc:
```cpp
#include <MicroOscUdp.h>
// The number 1024 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscUdp<1024> myOsc(&myUdp, mySendIp, mySendPort);
```

In `setup()` don't forget to start UDP:
```cpp
 myUdp.begin(myReceivePort);
```

## Receive OSC

### Defining a function for the reception of OSC messages

To receive OSC messages you must first create a function in which you will check the message address and get the message arguments:
```cpp
// FUNCTION THAT WILL BE CALLED WHEN AN OSC MESSAGE IS RECEIVED:
void myOscMessageParser( MicroOscMessage& receivedOscMessage) {
   // DO MESSAGE ADDRESS CHECKING AND ARGUMENT GETTING HERE
}
```
### Triggering the reception of OSC messages

In `loop()` you need to trigger the reception of the messages:
```cpp
myOsc.receiveMessages( myOscMessageParser );
```

### Check address and argument types of a MicroOscMessage

MicroOsc will return a reference to a `MicroOscMessage` when it receives an OSC message. **The following functions are members of `MicroOscMessage`.**

Check for an address:
```cpp
/**
* Returns true if the address matches exactly
*/
bool fullMatch(const char* address);
```

```cpp
if ( receivedOscMessage.fullMatch("/pot") ) {
	// ...
}
```

Check for an address more precisily:
```cpp
/**
* Returns true if the address and argument type tags match exactly.
*/
bool fullMatch(const char* address, const char * typetags);
```

```cpp
if ( receivedOscMessage.fullMatch("/pot", "i") ) {
	// ...
}
```
### Get argument values of a MicroOscMessage

MicroOsc will return a reference to a `MicroOscMessage` when it receives an OSC message.
**The following functions are members of `MicroOscMessage`.**

Get the next argument in the buffer as a 32-bit **int**:
```cpp
/**
* Returns the next argument as a 32-bit int. 
* Does not check buffer bounds.
*/
int32_t nextAsInt();
```
```cpp
int32_t intArgument = receivedOscMessage.nextAsInt();
```

Get the next argument in the buffer as a 32-bit **float**:
```cpp
/**
* Returns the next argument as a 32-bit float.
* Does not check buffer bounds.
*/
float nextAsFloat();
```
```cpp
float floatArgument = receivedOscMessage.nextAsFloat();
```

Get the next argument in the buffer as a C **string** pointer:
```cpp
/**
* Treats the next argument as a string and returns a pointer to the data as a C string, 
* or NULL if the buffer length is exceeded.
*/
const char* nextAsString();
```

```cpp
const char * s = receivedOscMessage.nextAsString();
```

Get the next argument in the buffer as a **byte array(blob)**:
```cpp
/**
* Treats the next argument as a blob of data and fills a pointer with the address to a byte array. 
* The pointer is NULL if there was an error.
* Returns the length of the byte blob. Returns 0 if there was an error.
*/
uint32_t nextAsBlob(const uint8_t **blobData);
```

```cpp
const uint8_t* blob;
uint32_t length = receivedOscMessage.nextAsBlob(&blob);
```

Get the next argument in the buffer as a **MIDI** data array:
```cpp
/**
* Treats the next value as MIDI and fills a pointer with the address to the MIDI data. 
* The pointer is NULL if the OSC bounds are exceeded.
* MIDI data always has a length of 4. Bytes from MSB to LSB are: port id, status byte, data1, data2
*/
int nextAsMidi(const uint8_t **midiData);
```  
```cpp
const uint8_t* midi;
receivedOscMessage.nextAsMidi(&midi);
```

