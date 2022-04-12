# MicroOsc

MicroOsc is a minimal [Open Sound Control](http://opensoundcontrol.org/) (OSC) library for Arduino. It is based on TinyOSC by Martin Roth (https://github.com/mhroth/TinyOSC). Given the limited nature of the library it also tends to be quite fast. It doesn't hold on to much state and it doesn't do much error checking. If you have a good idea of what OSC packets you will receive and need to process them quickly, this library might be for you.

## Supported Features
Due to its *micro* nature, MicroOsc does not support all standard OSC features. Currently it supports:
* message parsing
* message writing
* bundle parsing
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


It does not support:
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
