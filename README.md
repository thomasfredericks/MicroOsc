# MicroOsc

MicroOsc is a simple and lightweight [Open Sound Control](http://opensoundcontrol.org/) (OSC) library for Arduino. 
This simple and lightweight OSC library provides a unified API to work over UDP (Ethernet or Wifi and SLIP (Serial). 
It does not hold on to state and error checking is minimal.
A few Arduino and application (Pure Data, Max, Node JS) examples are included.
It is inspired on TinyOSC by Martin Roth (https://github.com/mhroth/TinyOSC).

## Supported Features
Currently MicroOsc supports:
* message parsing
* message writing
* bundle parsing as individual messages
* Send Types
  * `b`: binary blob
  * `f`: float
  * `d`: double
  * `i`: int (int32)
  * `h`: int64
  * `s`: string
  * `m`: midi

* Receive Types
  * `b`: binary blob
  * `f`: float
  * `d`: double
  * `i`: int (int32)
  * `s`: string
  * `m`: midi

## Unsupported Features
Currently MicroOsc does not support:
* ~~bundle writing~~
* ~~timetags~~
* ~~matching~~
* Send types not *yet* supported:
  * ~~`t`: timetag~~`
  * ~~``T`: true~~`
  * ~~``F`: false~~`
  * ~~``I`: infinitum~~`
  * ~~``N`: nil~~`
* Receive Types not *yet* supported:
  * ~~`h`: int64~~
  * ~~`t`: timetag~~
  * ~~`T`: true~~
  * ~~`F`: false~~
  * ~~`I`: infinitum~~
  * ~~`N`: nil~~
  
