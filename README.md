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

## Receive OSC


### Check for an adress. Returns `true` if the address matches exactly
```cpp
    /**
   * Returns true if the address and type tag matches.
   */
  bool fullMatch(const char* address);
```

### Check for an adress more precisily. Returns `true` if the address and typetags match exactly
```cpp
    /**
   * Returns true if the address and type tag matches.
   */
  bool fullMatch(const char* address, const char * typetags);
```

  /**
   * Returns the next 32-bit int. Does not check buffer bounds.
   */
  int32_t nextAsInt();


  /**
   * Returns the next 32-bit float. Does not check buffer bounds.
   */
  float nextAsFloat();


  /**
   * Returns the next string, or NULL if the buffer length is exceeded.
   */
  const char* nextAsString();

  /**
   * Points to the blob data. The pointer is NULL if there was an error.
   * Returns the length of the blob. Returns 0 if there was an error.
   */
  uint32_t nextAsBlob(const uint8_t **blobData);

  
  /**
   * Returns a pointer to the MIDI data. Returns NULL if the OSC buffer bounds are exceeded.
   * MIDI data always has a length of 4.
   */
  int nextAsMidi(const uint8_t **midiData);