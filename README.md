# MicroOsc

MicroOsc is a simple and lightweight [Open Sound Control](http://opensoundcontrol.org/) (OSC) library for Arduino. It is inspired on TinyOSC by Martin Roth (https://github.com/mhroth/TinyOSC). It doesn't hold on to state and error checking is minimal.

## Supported Features
Currently MicroOsc supports:
* message parsing
* message writing
* bundle parsing as individual messages
* Send Types
  * `b`: binary blob
  * `f`: float
  * `d`: double
  * `i`: int32
  * `h`: int64
  * `s`: string
  * `m`: midi
  * `t`: timetag
  * `T`: true
  * `F`: false
  * `I`: infinitum
  * `N`: nil
* Receive Types
  * `b`: binary blob
  * `f`: float
  * `i`: int32
  * `s`: string
  * `m`: midi

## Unsupported Features
Currently MicroOsc does not support:
* ~~bundle writing~~
* ~~timetags~~
* ~~matching~~
* Receive Types not *yet* supported:
  * ~~`d`: double~~
  * ~~`h`: int64~~
  * ~~`t`: timetag~~
  * ~~`T`: true~~
  * ~~`F`: false~~
  * ~~`I`: infinitum~~
  * ~~`N`: nil~~
