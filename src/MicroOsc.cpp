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
/*
#ifndef htonl
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#endif
*/


/*
 based on http://stackoverflow.com/questions/809902/64-bit-ntohl-in-c
 
 if the system is little endian, it will flip the bits
 if the system is big endian, it'll do nothing
 */
template<typename T> 
static inline T uOsc_bigEndian(const T& x)
{
    const int one = 1;
    const char sig = *(char*)&one;
    if (sig == 0) return x; // for big endian machine just return the input
    T ret;
    int size = sizeof(T);
    char* src = (char*)&x + sizeof(T) - 1;
    char* dst = (char*)&ret;
    while (size-- > 0){
        *dst++ = *src--;
    }
    return ret;
}


#include "MicroOsc.h"

void MicroOsc::padTheSize() {
  while ( (outputWritten % 4 ) ) {
      output->write(nullChar);
      outputWritten++; 
  }
}

void MicroOsc::writeAddress(const char *address) {

  outputWritten = 0;
    
  output->print(address);
  output->write(nullChar);
  outputWritten += strlen(address)+1;
  // pad the size
  padTheSize();
  
}

void MicroOsc::writeFormat(const char *format) {
  output->write(',');
  output->print(format);
  output->write(nullChar);
  outputWritten += strlen(format) + 2;

  // pad the size
  padTheSize();

}

void MicroOsc::writeInt(int32_t int32) {
  int32_t networkInt32 = uOsc_bigEndian(int32);
  //int32_t v32 = htonl(v);
  uint8_t * ptr = (uint8_t *) &networkInt32;
  output->write(ptr, 4);
  outputWritten += 4;
}

void MicroOsc::writeFloat(float f) {
  float v32 = uOsc_bigEndian(f);
  uint8_t * ptr = (uint8_t *) &v32;
  output->write(ptr, 4);
  outputWritten += 4;
}

void MicroOsc::writeDouble(double d) {
 double v64 = uOsc_bigEndian(d);
 uint8_t * ptr = (uint8_t *) &v64;
output->write(ptr, sizeof(double));
 outputWritten += sizeof(double);
}

void MicroOsc::writeString(const char *str){
  
  output->print(str);
  output->write(nullChar);
  outputWritten += strlen(str) + 1;
  // pad the size
  padTheSize();

}

void MicroOsc::writeBlob( unsigned char *b, int32_t length) {
  // Replace following three lines with writeInt
  uint32_t n32 = uOsc_bigEndian(length);
  uint8_t * ptr = (uint8_t *) &n32;
  output->write(ptr, 4);
  output->write(b, length);
  outputWritten += (4 + length);
  // pad the size
  padTheSize();
}
void MicroOsc::writeMidi(unsigned char *midi) {
 output->write(midi, 4);
 outputWritten += 4;
}

void MicroOsc::writeInt64(uint64_t h) {
 const uint64_t tBE = uOsc_bigEndian(h);
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
        writeInt(int32);
        break;
      }
      case 'b': {
        
        unsigned char *b = (unsigned char *) va_arg(ap, void *); // pointer to binary data
        const int32_t n = va_arg(ap, int32_t); // length of blob
        writeBlob(b,n);
        
        break;
      }
	  
      case 's': {
        const char *str = (const char *) va_arg(ap, void *);
        
        writeString(str);

        
        break;
      }
	  
	   case 'f': {
        double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
        writeFloat(d);
        break;
      }        

	   case 'd': {
        double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
        writeDouble(d);
        break;
      }  
      
	   case 'm': {
		 // get unsigned char array of size 4
        const unsigned char *const midi = (unsigned char *) va_arg(ap, void *);
       writeMidi(midi);
        break;
      }
	  
      case 'h': {
        const uint64_t h = (uint64_t) va_arg(ap, long long);
		    writeInt64(h);
        break;
      }
      case 't': // osc timetag
      case 'T': // true
      case 'F': // false
      case 'N': // nil
      case 'I': // infinitum
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
void MicroOsc::parseMessages(tOscCallbackFunction callback, unsigned char *buffer, const size_t bufferLength) {
 
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
    if ( parseMessage(buffer,bufferLength) == 0 ) callback(message);
  }
  
}

int MicroOsc::parseMessage(unsigned char  *buffer, const size_t bufferLength) {
  // NOTE(mhroth): if there's a comma in the address, that's weird
  size_t i = 0;
  while (buffer[i] != '\0') ++i; // find the null-terimated address
  while (buffer[i] != ',') ++i; // find the comma which starts the format string
  if (i >= bufferLength) return -1; // error while looking for format string
  // format string is null terminated
  message.format = (char*)(buffer + i + 1); // format starts after comma

  while (i < bufferLength && buffer[i] != '\0') ++i;
  if (i == bufferLength) return -2; // format string not null terminated

  i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
  message.marker = buffer + i;

  message.buffer = buffer;
  message.bufferLength = bufferLength;

  return 0;
}


/*
        double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
        double v64 = uOsc_bigEndian(d);
        uint8_t * ptr = (uint8_t *) &v64;
		*/

uint64_t MicroOsc::parseBundleTimeTag() {
  uint64_t timeTag = *((uint64_t *) (bundle.buffer+8));
  return uOsc_bigEndian(timeTag);
}


// check if first eight bytes are '#bundle '
bool MicroOsc::isABundle(const unsigned char  *buffer) {
  return (strcmp( (const char*)buffer, "#bundle") == 0); //return ((*(const int64_t *) buffer) == htonll(BUNDLE_ID));
}


void MicroOsc::parseBundle(unsigned char  *buffer, const size_t bufferLength) {
  bundle.buffer =  buffer;
  bundle.marker = buffer + 16; // move past '#bundle ' and timetag fields
  bundle.bufLen = bufferLength;
  bundle.bundleLen = bufferLength;
}





bool MicroOsc::getNextMessage() {
  if ((bundle.marker - bundle.buffer) >= bundle.bundleLen) return false;
  
  uint32_t lenBE = *((uint32_t *) bundle.marker);
  uint32_t bufferLength = uOsc_bigEndian(lenBE);
  
  parseMessage(bundle.marker+4, bufferLength);
  bundle.marker += (4 + bufferLength); // move marker to next bundle element
  return true;
}

void MicroOsc::sendMessage(const char *address, const char *format, ...) {
  beginMessage();
  va_list ap;
  va_start(ap, format);
  writeMessage( address, format, ap);
  va_end(ap);
  endMessage();
}


void MicroOsc::sendInt(const char *address, int32_t i){
  beginMessage();
  writeAddress(address);
  writeFormat("i");
  writeInt(i);
  endMessage();
}

void MicroOsc::sendFloat(const char *address,float f){
  beginMessage();
  writeAddress(address);
  writeFormat("f");
  writeFloat(f);
  endMessage();
}

void MicroOsc::sendString(const char *address,const char *str){
  beginMessage();
  writeAddress(address);
  writeFormat("s");
  writeString(str);
  endMessage();
}

void MicroOsc::sendBlob(const char *address,unsigned char *b, int32_t length){
  beginMessage();
  writeAddress(address);
  writeFormat("b");
  writeBlob(b,length);
  endMessage();
}

void MicroOsc::sendDouble(const char *address,double d){
  beginMessage();
  writeAddress(address);
  writeFormat("d");
  writeDouble(d);
  endMessage();
}

void MicroOsc::sendMidi(const char *address,unsigned char *midi){
  beginMessage();
  writeAddress(address);
  writeFormat("m");
  writeMidi(midi);
  endMessage();
}

void MicroOsc::sendInt64(const char *address,uint64_t h){
  beginMessage();
  writeAddress(address);
  writeFormat("h");
  writeInt64(h);
  endMessage();
}


MicroOscMessage::MicroOscMessage() {

}

int32_t MicroOscMessage::nextAsInt() {
  // convert from big-endian (network btye order)
  const int32_t iBE = *((int32_t *) marker);
  const int32_t i  = uOsc_bigEndian(iBE);
  
  marker += 4;
  return i;
}


float MicroOscMessage::nextAsFloat() {
  // convert from big-endian (network btye order)
  const uint32_t iBE = *((uint32_t *) marker);
  const uint32_t i  = uOsc_bigEndian(iBE);
  marker += 4;
  return *((float *) (&i)); // HARD CAST TO FLOAT
}


const char* MicroOscMessage::nextAsString() {
  int i = (int) strlen((const char*)marker);
  if (marker + i >= buffer + bufferLength) return NULL;
  const char *s = (const char*)marker;
  i = (i + 4) & ~0x3; // advance to next multiple of 4 after trailing '\0'
  marker += i;
  return s;
}



bool MicroOscMessage::fullMatch(const char* address) {

    return (strcmp( (const char *) buffer, address) == 0);
}

bool MicroOscMessage::fullMatch(const char* address, const char * typetags){
   return (strcmp( (const char*) buffer, address) == 0) && (strcmp( (const char*) format, typetags) == 0) ;
}

uint32_t MicroOscMessage::nextAsBlob( const unsigned char  **blob) {

  const uint32_t iBE = *((uint32_t *) marker);
  
  uint32_t length = 0;
  uint32_t i  = uOsc_bigEndian(iBE);
  
  if (marker + 4 + i <= buffer + bufferLength) { // not bigger than stored data
    length = i; // length of blob
    *blob = marker + 4;
    i = (i + 7) & ~0x3;
    marker += i;
  } else {
    length = 0;
    *blob = NULL;
  }

  return length;
}

int MicroOscMessage::nextAsMidi( const unsigned char  **midiData) {

  if (marker + 4  <= buffer + bufferLength) {
    *midiData = marker;
     marker += 4;
     return 4;
  } else {
    return 0;
  }
}




/*
size_t MicroOscMessage::nextAsBlob( const unsigned char  **blob, size_t maxLength) {
  size_t i = (size_t) ntohl(*((uint32_t *) message.marker)); // get the blob length
  if (i <= maxLength &&  message.marker + 4 + i <= message.buffer + message.bufferLength) {
    size_t bufferLength = i; // length of blob
    *blob = message.marker + 4;
    i = (i + 7) & ~0x3;
    message.marker += i;
    return bufferLength;
  } else {
    
    *blob = NULL;
    return 0;
  }
}
*/