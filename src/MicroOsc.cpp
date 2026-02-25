/* MicroOsc
 * By Thomas O Fredericks (tof@tofstuff.com)
 * Inspired by TinyOsc https://github.com/mhroth/TinyOsc by Martin Roth (mhroth@gmail.com)
 */

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <Udp.h>

#define OSC_TIMETAG_IMMEDIATELY 1L


#include "Arduino.h"
#include "MicroOscUtility.h"

#include "MicroOsc.h"

void MicroOsc::pad() {
  while ( (outputWritten % 4 ) ) {
    output->write(nullChar);
    outputWritten++;
  }
}

void MicroOsc::writeAddress(const char *address) {
  outputWritten = 0;
  output->print(address);
  output->write(nullChar);
  outputWritten += strlen(address) + 1;
  // pad the size
  pad();

}

void MicroOsc::writeFormat(const char *format) {
  output->write(',');
  output->print(format);
  output->write(nullChar);
  outputWritten += strlen(format) + 2;
  // pad the size
  pad();

}

void MicroOsc::messageAddInt(int32_t int32) {
  int32_t networkInt32 = swapBigEndian32(int32);
  //int32_t v32 = htonl(v);
  uint8_t * ptr = (uint8_t *) &networkInt32;
  output->write(ptr, 4);
  outputWritten += 4;
}

void MicroOsc::messageAddFloat(float f) {
  float v32 = swapBigEndian32(f);
  uint8_t * ptr = (uint8_t *) &v32;
  output->write(ptr, 4);
  outputWritten += 4;
}

void MicroOsc::messageAddDouble(double d) {
  double v64 = swapBigEndian64(d);
  uint8_t * ptr = (uint8_t *) &v64;
  output->write(ptr, sizeof(double));
  outputWritten += sizeof(double);
}

void MicroOsc::messageAddString(const char *str) {

  output->print(str);
  output->write(nullChar);
  outputWritten += strlen(str) + 1;
  // pad the size
  pad();

}

void MicroOsc::messageAddBlob( const uint8_t * b, int32_t length) {
  // Replace following three lines with messageAddInt
  uint32_t n32 = swapBigEndian32(length);
  uint8_t * ptr = (uint8_t *) &n32;
  output->write(ptr, 4);
  output->write(b, length);
  outputWritten += (4 + length);
  // pad the size
  pad();
}
void MicroOsc::messageAddMidi(const unsigned char *midi) {
  output->write(midi, 4);
  outputWritten += 4;
}

void MicroOsc::messageAddInt64(uint64_t h) {
  const uint64_t tBE = swapBigEndian64(h);
  uint8_t * ptr = (uint8_t *) &tBE;
  output->write(ptr, 8);
  outputWritten += 8;
}



void MicroOsc::writeMessage( const char *address, const char *format, va_list ap) {

  writeAddress( address);

  writeFormat( format);

  for (int j = 0; format[j] != '\0'; ++j) {

    switch (format[j]) {
    case 'i': {
      const int32_t  int32 = va_arg(ap, int32_t);
      messageAddInt(int32);
      break;
    }
    case 'b': {

      unsigned char *b = (unsigned char *) va_arg(ap, void *); // pointer to binary data
      const int32_t n = va_arg(ap, int32_t); // length of blob
      messageAddBlob(b, n);

      break;
    }

    case 's': {
      const char *str = (const char *) va_arg(ap, void *);

      messageAddString(str);


      break;
    }

    case 'f': {
      double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
      messageAddFloat(d);
      break;
    }

    case 'd': {
      double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
      messageAddDouble(d);
      break;
    }

    case 'm': {
      // get unsigned char array of size 4
      const unsigned char *const midi = (unsigned char *) va_arg(ap, void *);
      messageAddMidi(midi);
      break;
    }

    case 'h': {
      const uint64_t h = (uint64_t) va_arg(ap, long long);
      messageAddInt64(h);
      break;
    }
    case 'T': // true
    case 'F': // false
    case 'N': // nil
    case 'I': {  // Impulse
      // No argument
      break;
    }

    case 't': // osc timetag
    default:
      // unsupported type, force an error (length will not be a multiple of 4)
      output->write(nullChar);
    }
  }

}





MicroOsc::MicroOsc(Print* output) {
  this->output = output;
};





// http://opensoundcontrol.org/spec-1_0
void MicroOsc::parseMessages(MicroOscCallback callback, unsigned char *buffer, const size_t bufferLength) {

  if ( callback == NULL ) return;

  // Check for bundles
  if (isABundle(buffer)) {
    parseBundle(buffer, bufferLength);
    timetag = parseBundleTimeTag();
    //isPartOfABundle = true;
    while ( getNextMessage()) {
      callback(message);
    }
  } else {
    timetag = 0;
    //isPartOfABundle = false;
    if ( message.parseMessage(buffer, bufferLength) == 0 ) {
      callback(message);
    }
  }

}

void MicroOsc::parseMessages(MicroOscCallbackWithSource callback, unsigned char *buffer, const size_t bufferLength) {

  if ( callback == NULL ) return;

  // Check for bundles
  if (isABundle(buffer)) {
    parseBundle(buffer, bufferLength);
    timetag = parseBundleTimeTag();
    //isPartOfABundle = true;
    while ( getNextMessage()) {
      callback(*this, message);
    }
  } else {
    timetag = 0;
    //isPartOfABundle = false;
    if ( message.parseMessage(buffer, bufferLength) == 0 ) {
      callback(*this, message);
    }
  }

}



/*
        double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
        double v64 = uOsc_bigEndian(d);
        uint8_t * ptr = (uint8_t *) &v64;
    */

uint64_t MicroOsc::parseBundleTimeTag() {
  uint64_t timeTag = *((uint64_t *) (bundle.buffer + 8));
  return swapBigEndian64(timeTag);
}


// check if first eight bytes are '#bundle '
bool MicroOsc::isABundle(const unsigned char  *buffer) {
  return (strcmp( (const char*)buffer, "#bundle") == 0); //return ((*(const int64_t *) buffer) == htonll(BUNDLE_ID));
}


void MicroOsc::parseBundle(unsigned char  *buffer, const size_t bufferLength) {
  bundle.buffer =  buffer;
  bundle.marker = buffer + 16; // move past '#bundle ' and timetag fields
  //bundle.bufLen = bufferLength;
  bundle.bundleLen = bufferLength;
}





bool MicroOsc::getNextMessage() {
  if ((int32_t)(bundle.marker - bundle.buffer) >= bundle.bundleLen) return false;

  uint32_t lenBE = *((uint32_t *) bundle.marker);
  uint32_t bufferLength = swapBigEndian32(lenBE);

  message.parseMessage(bundle.marker + 4, bufferLength);
  bundle.marker += (4 + bufferLength); // move marker to next bundle element
  return true;
}

void MicroOsc::sendMessage(const char *address, const char *format, ...) {
  if ( transportReady() ) {
    transportBegin();
    va_list ap;
    va_start(ap, format);
    writeMessage( address, format, ap);
    va_end(ap);
    transportEnd();
  }
}

void MicroOsc::sendWithoutArguments(const char *address, const char * type) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat(type);
    transportEnd();
  }
}

void MicroOsc::sendImpulse(const char *address) {
  sendWithoutArguments(address,"I");
}

void MicroOsc::sendTrue(const char *address) {
  sendWithoutArguments(address,"T");
}

void MicroOsc::sendFalse(const char *address) {
  sendWithoutArguments(address,"F");
}

void MicroOsc::sendNull(const char *address) {
  sendWithoutArguments(address,"N");
}


void MicroOsc::sendInt(const char *address, int32_t i) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat("i");
    messageAddInt(i);
    transportEnd();
  }
}

void MicroOsc::sendFloat(const char *address, float f) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat("f");
    messageAddFloat(f);
    transportEnd();
  }
}

void MicroOsc::sendString(const char *address, const char *str) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat("s");
    messageAddString(str);
    transportEnd();
  }
}

void MicroOsc::sendBlob(const char *address, const uint8_t *b, int32_t length) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat("b");
    messageAddBlob(b, length);
    transportEnd();
  }
}

void MicroOsc::sendDouble(const char *address, double d) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat("d");
    messageAddDouble(d);
    transportEnd();
  }
}

void MicroOsc::sendMidi(const char *address, unsigned char *midi) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat("m");
    messageAddMidi(midi);
    transportEnd();
  }
}

void MicroOsc::sendInt64(const char *address, uint64_t h) {
  if ( transportReady() ) {
    transportBegin();
    writeAddress(address);
    writeFormat("h");
    messageAddInt64(h);
    transportEnd();
  }
}

