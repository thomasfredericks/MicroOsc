# MicroOsc

- MicroOsc is a simple and lightweight [Open Sound Control](http://opensoundcontrol.org/) (OSC) library for the Arduino frameworks that supports Arduino platforms. 
- MicroOsc provides a unified API to work over Serial (SLIP) and over UDP (Ethernet or Wifi).
- MicroOsc does not hold on to state. Error checking is minimal.
- MicroOsc was inspired by [TinyOSC](https://github.com/mhroth/TinyOSC) by Martin Roth.

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
  * `I`: impulse (message with no arguments; OSC v1.1)
  * `T`: TRUE (message with no arguments)
  * `F`: FALSE (message with no arguments)
  * `N`: NULL (message with no arguments)

* Receive Types
  * `b`: blob (byte array)
  * `f`: float
  * `d`: double
  * `i`: int (int32)
  * `s`: string
  * `m`: midi

## Unsupported Features

MicroOsc will *eventually* support:
- bundle writing (receiving is supported)

Send types not *yet* supported:
- `t`: timetag

Receive Types not *yet* supported:
- `h`: int64
- `t`: timetag
- `T`: true
- `F`: false
- `I`: impulse
- `N`: nil
  
MicroOsc will probably never support:
- Timetags
- Regular expression matching


## Initialization  

There are *currently* 2 supported transport protocols that are identical except for their initialization:
- Serial (with SLIP)
- UDP (Ethernet or WiFi).

### OSC SLIP

```cpp
#include <MicroOscSlip.h>
MicroOscSlip<128> myOsc(&Serial); // <#> : # of bytes reserved for incomming messages.
```
In `setup()` don't forget to start Serial:
```cpp
  Serial.begin(115200);
```

### OSC UDP

Initialize UDP and network details first.

For WiFi:
```cpp
#include <WiFiUdp.h>
WiFiUDP myUdp;
unsigned int myReceivePort = 8888;
IPAddress myDestinationIp(192, 168, 1, 210);
unsigned int myDestinationPort = 7777;
```

For Ethernet:
```cpp
#include <Ethernet.h>
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP myUdp;
unsigned int myReceivePort = 8888;
IPAddress myDestinationIp(192, 168, 1, 210);
unsigned int myDestinationPort = 7777;
```

Include `MicroOsc` and initiliaze yout instance of `MicroOsc`.

You can initiliaze `MicroOsc` **with** the destination:
```cpp
#include <MicroOscUdp.h>
MicroOscUdp<1024> myOsc(&myUdp, myDestinationIp, myDestinationPort); // <#> : # of bytes reserved for incomming messages.
```

Or, if you need to set it alter, you can also initiliaze `MicroOsc` **without** the destination:
```cpp
#include <MicroOscUdp.h>
MicroOscUdp<1024> myOsc(&myUdp); // <#> : # of bytes reserved for incomming messages.
```

In `setup()` don't forget to start your UDP instance:
```cpp
 myUdp.begin(myReceivePort);
```
- `myReceivePort` : port number (`unsigned int`) that this device (the microcontroller) should listen to.

The destination can be changed during runtime:
```cpp
myOsc.setDestination( myDestinationIp, myDestinationPort) 
```
- `myDestinationIp` : IP address (`IPAddress`) of the device you want to send messages to.
- `myDestinationPort` : port number (`unsigned int`) of the device you want to send messages to.


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
myOsc.onOscMessageReceived( myOscMessageParser );
```

### Check address and argument types of a MicroOscMessage

MicroOsc will return a reference to a `MicroOscMessage` when it receives an OSC message. **The following functions are members of `MicroOscMessage`.**

#### Check address

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

####  Check address and type tags
```cpp
/**
* Returns true if the address and argument type tags match exactly.
*/
bool checkOscAddressAndTypeTags(const char* address, const char * typetags);
```

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
if ( receivedOscMessage.checkOscAddressAndTypeTags("/pot", "i") ) {
  // ...
}
```
### Get arguments of a MicroOscMessage

MicroOsc will return a reference to a `MicroOscMessage` when it receives an OSC message.
**The following functions are members of `MicroOscMessage`.**

#### Get next as a 32-bit **int**
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

####  Get next as a 32-bit **float**
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

####  Get next argument as a C **string** pointer

```cpp
/**
* Treats the next argument as a string and returns a pointer to the data as a C string, 
* or NULL if the buffer length is exceeded.
*/
const char* nextAsString();
```

**WARNING**: Do not store the pointer returned by this function. Only use it as read only inside of the function called by `onOscMessageReceived()`.

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
const char * s = receivedOscMessage.nextAsString();
```

#### Get next as a **byte array(blob)** 

```cpp
/**
* Treats the next argument as a blob of data and sets a pointer with the address to a byte array. 
* The pointer is NULL if there was an error.
* Returns the length of the byte blob. Returns 0 if there was an error.
*/
uint32_t nextAsBlob(const uint8_t **blobData);
```
**WARNING**: Do not store the pointer returned by this function. Only use it as read only inside of the function called by `onOscMessageReceived()`.

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
const uint8_t* blob;
uint32_t length = receivedOscMessage.nextAsBlob(&blob);
```

####  Get next as a **MIDI** data array

```cpp
/**
* Treats the next value as MIDI and sets a pointer with the address to the MIDI data. 
* The pointer is NULL if the OSC bounds are exceeded.
* MIDI data always has a length of 4. Bytes from MSB to LSB are: port id, status byte, data1, data2
*/
int nextAsMidi(const uint8_t **midiData);
```
**WARNING**: Do not store the pointer returned by this function. Only use it as read only inside of the function called by `onOscMessageReceived()`.

Example with a `MicroOscMessage` named `receivedOscMessage`:
```cpp
const uint8_t* midi;
receivedOscMessage.nextAsMidi(&midi);
```

#### Parse a list of arguments

You can also receive lists of arguments with MicroOsc. Everytime you get the value of an argument, an internal pointer moves to the next argument in the list automatically.

For example if you to receive and parse the OSC message `/controller ["FREQ",  0.125, "kHz"]` you can do the following:
```cpp
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

## Send OSC

MicroOsc provides individual functions for sending a single argument tp all supported types. It also provides an advanced function for sending messages with multiple arguments of the same or mixed types.

### Sending a single argument

Use one of these to send a single argument:
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

Example of sending an int:
```cpp
int reading = analogRead(1);
myOsc.sendInt("/photo", reading);
``` 

### Sending a list of arguments

MicroOsc can sends lists of variable arguments, but because we are in **C**, you must **cast** your arguments (especially `int` as some Arduino boards use `uint16_t` and others `uint32_t`) properly before sending them.

To send a list, use `sendMessage()`:
```cpp
void sendMessage(const char *address, const char *format, ... );
``` 
The format string defines the argument type of each following argument. You must provide a number of arguments depending on each argument type:
* "i" : one `int32_t`
* "f" : one 32-bit `float`
* "s" : one pointer to a **null terminated** char array
* "b" : one pointer to a `uint8_t` array **followed by** its `int32_t` length 
* "m" : one pointer to a `uint8_t` array of size 4

Example that sends a "/stuff" message with a float, string and integer arguments:
```cpp
myOsc.sendMessage("/stuff", "fsi", (float) 1.0 , "hello", (int32_t) 2);
``` 

Example that sends a "/blub" message with a blob argument:
```cpp
uint8_t blob[4] = {1,2,3,4};
uint32_t length = 4;
myOsc.sendMessage("/blub", "b", blob, (int32_t) length);
```

## Full API

### Classes

MicroOsc contains two core classes:

- `MicroOsc`  
  The main OSC interface used to send and receive OSC messages. It handles message encoding, transport handling, bundle parsing, and dispatching received messages.

- `MicroOscMessage`  
  Represents a single received OSC message. It provides methods to inspect the OSC address, verify argument types, and sequentially read message arguments.

### Overview of OSC receiving methods of MicroOsc

| MicroOsc Method | Description |
| --------------- | --------------- |
| `void parseMessages(MicroOscCallback callback, unsigned char *buffer, size_t bufferLength)` | Parses OSC data contained in `buffer` and calls `callback` once for each received message. Supports bundles and single messages. |
| `void parseMessages(MicroOscCallbackWithSource callback, unsigned char *buffer, size_t bufferLength)` | Same as above but also passes the `MicroOsc` instance to the callback, allowing access to additional context such as timetag or transport information. |

### Overview of all OSC parsing methods of MicroOscMessage

`MicroOscMessage` reads arguments sequentially. Each call to a `nextAs*()` method advances the internal read pointer to the next argument. Arguments must be read in the same order as they were sent.

| MicroOscMessage Method | Description |
| --------------- | --------------- |
| `const char* getOscAddress()` | Returns a pointer to the OSC address string contained in the message. |
| `bool checkOscAddress(const char* address)` | Returns `true` if the OSC address matches exactly. |
| `bool checkOscAddressAndTypeTags(const char* address, const char * typetags)` | Returns `true` if both the OSC address and argument type tags match exactly. |
| `int32_t nextAsInt()` | Returns the next argument as a 32-bit integer. |
| `float nextAsFloat()` | Returns the next argument as a 32-bit float. |
| `double nextAsDouble()` | Returns the next argument as a 64-bit double. |
| `const char* nextAsString()` | Treats the next argument as a C string and returns a pointer to the internal data buffer. |
| `uint32_t nextAsBlob(const uint8_t **blobData)` | Treats the next argument as a blob. Writes a pointer to the blob data and returns its length in bytes. |
| `int nextAsMidi(const uint8_t **midiData)` | Treats the next argument as a MIDI message. Writes a pointer to the 4-byte MIDI data and returns the size (4). |

#### Advanced MicroOscMessage methods

| Advanced MicroOscMessage Method | Description |
| --------------- | --------------- |
| `void copyAddress(char * destinationBuffer, size_t destinationBufferMaxLength)` | Copies the OSC address into a user-provided buffer with a maximum length. |
| `void copyTypeTags(char * destinationBuffer, size_t destinationBufferMaxLength)` | Copies the OSC type tags into a user-provided buffer with a maximum length. |

### Overview of all sending OSC methods of MicroOsc

All sending functions internally write the OSC address, type tags, arguments, and handle padding according to the OSC specification. Messages are only sent if the transport is ready.

| MicroOsc Method | Description |
| --------------- | --------------- |
| `void sendInt(const char *address, int32_t i)` | Sends a single integer OSC message. |
| `void sendFloat(const char *address, float f)` | Sends a single float OSC message. |
| `void sendString(const char *address, const char *str)` | Sends a single string OSC message. |
| `void sendBlob(const char *address, unsigned char *b, int32_t length)` | Sends a single blob (array of bytes) OSC message. |
| `void sendDouble(const char *address, double d)` | Sends a single double OSC message. |
| `void sendMidi(const char *address, unsigned char *midi)` | Sends a single MIDI OSC message (4 bytes). |
| `void sendInt64(const char *address, uint64_t h)` | Sends a single 64-bit integer OSC message. |
| `void sendImpulse(const char *address)` | Sends an OSC impulse message (no arguments, type tag `I`). |
| `void sendTrue(const char *address)` | Sends an OSC boolean true message (type tag `T`). |
| `void sendFalse(const char *address)` | Sends an OSC boolean false message (type tag `F`). |
| `void sendNull(const char *address)` | Sends an OSC nil message (type tag `N`). |
| `void sendMessage(const char *address, const char *format, ...)` | Sends an OSC message with multiple arguments. The `format` string defines argument types using OSC type tags. |

### Dynamic message building

These methods allow manual construction of an OSC message by explicitly starting and ending it. **The argument count and types must be managed manually; MicroOsc does not perform type or count checking.**

| MicroOsc Method | Description |
| --------------- | --------------- |
| `void messageBegin(const char *address, const char *format)` | Begins a new OSC message. Writes the OSC address, and writes the type tag string (`format`). After this, use one or more `messageAdd*()` methods to append arguments. These calls must match the format. |
| `void messageEnd()` | Ends the current OSC. This finalizes and sends the message through the active transport. |

Methods for adding arguments to a message:

| MicroOsc Method | Description |
| --------------- | --------------- |
| `void messageAddInt(int32_t value)` | Appends a 32-bit integer argument in big-endian format. |
| `void messageAddFloat(float value)` | Appends a 32-bit float argument in big-endian format. |
| `void messageAddDouble(double value)` | Appends a 64-bit double argument in big-endian format. |
| `void messageAddString(const char *str)` | Appends a string argument. The string is null-terminated and padded to a multiple of 4 bytes. |
| `void messageAddBlob(unsigned char *data, int32_t length)` | Appends a blob argument. Writes the length as a 32-bit integer, followed by the raw data, then pads to a multiple of 4 bytes. |
| `void messageAddMidi(const unsigned char *midi)` | Appends a 4-byte MIDI argument. |
| `void messageAddInt64(uint64_t value)` | Appends a 64-bit integer argument in big-endian format. |

### Supported OSC type tags

The following type tags are supported when sending or receiving messages:

| Type Tag | Description |
| --------------- | --------------- |
| `i` | 32-bit integer |
| `f` | 32-bit float |
| `d` | 64-bit double |
| `s` | String |
| `b` | Blob |
| `m` | MIDI message (4 bytes) |
| `h` | 64-bit integer |
| `T` | Boolean true (no argument data) |
| `F` | Boolean false (no argument data) |
| `N` | Nil (no argument data) |
| `I` | Impulse (no argument data) |

### Notes

- OSC data is encoded using big-endian (network byte order).
- Arguments must be read in the same order as defined by the type tags.
- Returned pointers from `nextAsString()`, `nextAsBlob()`, and `nextAsMidi()` refer to internal buffers and must not be modified or stored beyond the lifetime of the message.
- Bundles are automatically detected and unpacked. Each contained message triggers the callback individually.

