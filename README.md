# MicroOsc

MicroOsc is a simple and lightweight [Open Sound Control](http://opensoundcontrol.org/) (OSC) library for the Arduino frameworks that supports Arduino, Teensy, esp8266 and ESP32 platforms. 

MicroOsc provides a unified API to work over Serial (SLIP) and over UDP (Ethernet or Wifi).

It does not hold on to state and error checking is minimal.

Examples are provided for Arduino and a few applications (Pure Data, Max, Node JS).

MicroOsc was inspired by [TinyOSC](https://github.com/mhroth/TinyOSC) by Martin Roth.

# Supported Features
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

# Unsupported Features

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

# Classes

MicroOsc contains 2 classes:
- `MicroOsc` : the main Class
- `MicroOscMessage` : a received OSC message

## Overview of all OSC receiving methods of MicroOsc
| MicroOsc  Method | Description |
| --------------- | --------------- |
| `void onOscMessageReceived(callback)` | Check for messages and execute callback for every received message |


## Overview of all OSC parsing methods of MicroOscMessage
| MicroOscMessage Method | Description |
| --------------- | --------------- |
| `void onOscMessageReceived(callback)` | Check for messages and execute callback for every received message |
| `bool checkOscAddress(const char* address)` | Returns `true` if the address matches exactly |
| `bool checkOscAddressAndTypeTags(const char* address,const char * typetags)` | Returns `true` if the address and argument type tags match exactly |
| `int32_t nextAsInt()` | Returns the next argument as a 32-bit int |
| `float nextAsFloat()` | Returns the next argument as a 32-bit float |
| `const char* nextAsString()` | Treats the next argument as a C string and returns a pointer to the data |
| `uint32_t nextAsBlob(const uint8_t **blobData)` | Treats the next argument as a blob of data and fills a pointer with the address to a byte array |
| `int nextAsMidi(const uint8_t **midiData)` | Treats the next value as MIDI and fills a pointer with the address to the MIDI data |

### Advanced MicroOscMessage methods
| Advanced MicroOscMessage Method | Description |
| --------------- | --------------- |
| `size_t copyAddress(char * destinationBuffer, size_t destinationBufferMaxLength)` | Copies the address into a `char*` destinationBuffer of maximum length destinationBufferMaxLength. Returns the number of characters copied |
| `size_t copyTypeTags(char * destinationBuffer, size_t destinationBufferMaxLength)` | Copies the type tags into a `char*` destinationBuffer of maximum length destinationBufferMaxLength. Returns the number of characters copied |


## Overview of all sending OSC methods of MicroOsc
| MicroOsc  Method | Description |
| --------------- | --------------- |
| `void sendInt(const char *address, int32_t i)` | Send a single int OSC message |
| `void sendFloat(const char *address, float f);` | Send a single float OSC message |
| `void sendString(const char *address, const char *str)` | Send a single string OSC message |
| `void sendBlob(const char *address, unsigned char *b, int32_t length)` | Send a single blob (array of bytes) OSC message |
| `void sendDouble(const char *address,double d)` | Send a single double OSC message |
| `void sendMidi(const char *address,unsigned char *midi)` |  Send a single MIDI OSC message |
| `void sendInt64(const char *address, uint64_t h)` | Send a single Int64 OSC message |
| `void sendMessage(const char *address, const char *format, ...)` | Send an OSC message with any mnumber of arguments of diffrent types |

# Initialization  

There are currently 2 supported transport protocols. Serial (with SLIP) and UDP (Ethernet or WiFi). The 2 versions are identical except for their initialization.

## OSC SLIP

```cpp
#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myOsc(&Serial);
```

In `setup()` don't forget to start Serial:
```cpp
  Serial.begin(115200);
```

## OSC UDP

Initialize UDP and network details first:
```cpp
#include <WiFiUdp.h>
WiFiUDP myUdp;
unsigned int myReceivePort = 8888;
IPAddress mySendIp(192, 168, 1, 210);
unsigned int mySendPort = 7777;
```

Initialize and include MicroOsc:
```cpp
#include <MicroOscUdp.h>
// The number 1024 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscUdp<1024> myOsc(&myUdp, mySendIp, mySendPort);
```

In `setup()` don't forget to start your UDP instance:
```cpp
 myUdp.begin(myReceivePort);
```

The destination can be changed during runtime:
```cpp
myOsc.setDestination(IPAddress destinationIp, unsigned int destinationPort) 
```
# Receive OSC

## Defining a function for the reception of OSC messages

To receive OSC messages you must first create a function in which you will check the message address and get the message arguments:
```cpp
// FUNCTION THAT WILL BE CALLED WHEN AN OSC MESSAGE IS RECEIVED:
void myOscMessageParser( MicroOscMessage& receivedOscMessage) {
   // DO MESSAGE ADDRESS CHECKING AND ARGUMENT GETTING HERE
}
```
## Triggering the reception of OSC messages

In `loop()` you need to trigger the reception of the messages:
```cpp
myOsc.onOscMessageReceived( myOscMessageParser );
```

## Check address and argument types of a MicroOscMessage

MicroOsc will return a reference to a `MicroOscMessage` when it receives an OSC message. **The following functions are members of `MicroOscMessage`.**

### Check for an address

```cpp
/**
* Returns true if the address matches exactly
*/
bool checkOscAddress(const char* address);
```

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
if ( receivedOscMessage.checkOscAddress("/pot") ) {
  // ...
}
```

###  Check for an address more precisily
```cpp
/**
* Returns true if the address and argument type tags match exactly.
*/
bool checkOscAddressAndTypeTags(const char* address, const char * typetags);
```

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
if ( checkOscAddressAndTypeTags.checkOscAddress("/pot", "i") ) {
  // ...
}
```
## Get argument values of a MicroOscMessage

MicroOsc will return a reference to a `MicroOscMessage` when it receives an OSC message.
**The following functions are members of `MicroOscMessage`.**

### Get the next argument in the buffer as a 32-bit **int**
```cpp
/**
* Returns the next argument as a 32-bit int. 
* Does not check buffer bounds.
*/
int32_t nextAsInt();
```

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
int32_t intArgument = receivedOscMessage.nextAsInt();
```

###  Get the next argument in the buffer as a 32-bit **float**
```cpp
/**
* Returns the next argument as a 32-bit float.
* Does not check buffer bounds.
*/
float nextAsFloat();
```

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
float floatArgument = receivedOscMessage.nextAsFloat();
```

###  Get the next argument in the buffer as a C **string** pointer

```cpp
/**
* Treats the next argument as a string and returns a pointer to the data as a C string, 
* or NULL if the buffer length is exceeded.
*/
const char* nextAsString();
```

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
const char * s = receivedOscMessage.nextAsString();
```

### Get the next argument in the buffer as a **byte array(blob)**
```cpp
/**
* Treats the next argument as a blob of data and sets a pointer with the address to a byte array. 
* The pointer is NULL if there was an error.
* Returns the length of the byte blob. Returns 0 if there was an error.
*/
uint32_t nextAsBlob(const uint8_t **blobData);
```

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
const uint8_t* blob;
uint32_t length = receivedOscMessage.nextAsBlob(&blob);
```

###  Get the next argument in the buffer as a **MIDI** data array
```cpp
/**
* Treats the next value as MIDI and sets a pointer with the address to the MIDI data. 
* The pointer is NULL if the OSC bounds are exceeded.
* MIDI data always has a length of 4. Bytes from MSB to LSB are: port id, status byte, data1, data2
*/
int nextAsMidi(const uint8_t **midiData);
```  

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
const uint8_t* midi;
receivedOscMessage.nextAsMidi(&midi);
```

### Parse a list of arguments

You can also receive lists of arguments with MicroOsc. Everytime you get the value of an argument, an internal pointer moves to the next argument in the list automatically.

For example if you to receive and parse the OSC message `/controller ["FREQ",  0.125, "kHz"]` you can do the following:
```
//  OSC MSG "/controller" WITH THE ARGUMENT LIST "sfs" (string, float, string)
if ( receivedOscMessage.checkOscAddress("/controller", "sfs") ) {

  // GET THE FIRST AS A STRING
  const char * firstAsString = receivedOscMessage.nextAsString();

 // GET THE SECOND AS A FLOAT
  float secondAsFloat  = receivedOscMessage.nextAsFloat();

  // GET THE THIRD AS A STRING
  const char * thirdAsAString = receivedOscMessage.nextAsString();
}
```

# Send OSC

MicroOsc provides individual functions for sending a single argument tp all supported types. It also provides an advanced function for sending messages with multiple arguments of the same or mixed types.

## Sending single argument messages

```cpp
void sendMessage(const char *address, const char *format, ...);
void sendInt(const char *address, int32_t i);
void sendFloat(const char *address, float f);
void sendString(const char *address, const char *str);
void sendBlob(const char *address, unsigned char *b, int32_t length);
void sendDouble(const char *address,double d);
void sendMidi(const char *address,unsigned char *midi);
void sendInt64(const char *address, uint64_t h);
``` 

Example of sending an int :
```cpp
int reading = analogRead(1);
myOsc.sendInt("/photo", reading);
``` 

## Sending of lists of arguments

MicroOsc can sends lists of variable arguments, but because we are in **C**, you must **cast** your arguments (especially `int` as some Arduino boards use `uint16_t` and others `uint32_t`) properly before sending them.

To send a list, use `sendMessage()`:
```cpp
void sendMessage(const char *address, const char *format, ... );
``` 
The format string defines the argument type of each following argument. You must provide a number of arguments depending on each argument type:
* "i" : one `uint32_t`
* "f" : one 32-bit `float`
* "s" : one pointer to a **null terminated** char array
* "b" : one pointer to a `uint8_t` array **followed by** its `uint32_t` length 
* "m" : one pointer to a `uint8_t` array of size 4

Example that sends a "/stuff" message with a float, string and integer arguments:
```cpp
myOsc.sendMessage("/stuff", "fsi", (float) 1.0 , "hello", (uint32_t) 2);
``` 

Example that sends a "/blub" message with a blob argument:
```cpp
uint8_t blob[4] = {1,2,3,4};
uint32_t length = 4;
myOsc.sendMessage("/blub", "b", blob, (uint32_t) length);
``` 

