/* MicroOsc
 * By Thomas O Fredericks (tof@tofstuff.com) 
 * Inspired by TinyOsc https://github.com/mhroth/TinyOsc by Martin Roth (mhroth@gmail.com)
 */

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <Udp.h>

#define MicroOsc_TIMETAG_IMMEDIATELY 1L


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


size_t MicroOsc::vprint(Print* output, const char *address, const char *format, va_list ap) {
  
  uint8_t nullChar = '\0';

  size_t i = strlen(address);
  if (address == NULL ) return -1;
  output->print(address);
  output->write(nullChar);
  i++; 

  // pad the size
  while ( (i % 4 ) ) {
      output->write(nullChar);
      i++; 
  }
  
  output->write(',');
  i++; 

  size_t s_len = strlen(format);
  if (format == NULL ) return -2;
  output->print(format);
  output->write(nullChar);
  i += s_len + 1;

   // pad the size
  while ( (i % 4 ) ) {
      output->write(nullChar);
      i++; 
  }


  for (int j = 0; format[j] != '\0'; ++j) {
	  
    switch (format[j]) {
	  case 'i': {
		const int32_t  v = va_arg(ap, int);
        int32_t v32 = uOsc_bigEndian(v);
       //int32_t v32 = htonl(v);
        uint8_t * ptr = (uint8_t *) &v32;
       output->write(ptr, 4);
       i+=4;
       //Serial.println();
       //  Serial.print(" ");
        //    Serial.println(v32);
        //    Serial.print("v32 ");
        //    Serial.println(v32);
        break;
      }
      case 'b': {
        const int n = va_arg(ap, int); // length of blob
        
        uint32_t n32 = uOsc_bigEndian(n);
        uint8_t * ptr = (uint8_t *) &n32;
        output->write(ptr, 4);

        unsigned char *b = (unsigned char *) va_arg(ap, void *); // pointer to binary data
        output->write(b, n);

        i += (4 + n);
        
        break;
      }
	  
      case 's': {
        const char *str = (const char *) va_arg(ap, void *);
        s_len = strlen(str);
        output->print(str);
          output->write(nullChar);
         i += s_len + 1;
        break;
      }
	  
	   case 'f': {
        double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
        float v = d; // use float as double might be 64 bytes
        float v32 = uOsc_bigEndian(v);
        uint8_t * ptr = (uint8_t *) &v32;
        output->write(ptr, 4);
        i+=4;
        break;
      }        

	   case 'd': {
        double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
        double v64 = uOsc_bigEndian(d);
        uint8_t * ptr = (uint8_t *) &v64;
        output->write(ptr, sizeof(double));
        i+=sizeof(double);
        break;
      }  
      
	   case 'm': {
		 // get unsigned char array of size 4
        const unsigned char *const k = (unsigned char *) va_arg(ap, void *);
        output->write(k, 4);
        i += 4;
        break;
      }
	  case 't':
      case 'h': {
        const uint64_t t = (uint64_t) va_arg(ap, long long);
		const uint64_t tBE = uOsc_bigEndian(t);
		uint8_t * ptr = (uint8_t *) &tBE;
        output->write(ptr, sizeof(uint64_t));
        i += sizeof(uint64_t);
        break;
      }
      
      case 'T': // true
      case 'F': // false
      case 'N': // nil
      case 'I': // infinitum
          break;
      default: return 0; // unsupported type
    }
	// pad the size
	while ( (i % 4 ) ) {
		output->write(nullChar);
		i++; 
	}
  }

  return i; // return the total number of bytes written
}


void MicroOsc::writeMessage(Print* output, const char *address, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vprint(output, address, format, ap);
  va_end(ap);
}



MicroOsc::MicroOsc(){
  //b = &bundle;
  //o = &message;

}



// http://opensoundcontrol.org/spec-1_0
void MicroOsc::parseMessages(tOscCallbackFunction callback, unsigned char *buffer, const size_t len) {
 
  if ( callback == NULL ) return; 

  // Check for bundles
  if (isABundle(buffer)) {

    parseBundle(buffer, len);
    timetag = parseBundleTimeTag();
    isPartOfABundle = true;
    
    while ( getNextMessage()) {
      callback(message);
    }
  } else {
    timetag = 0;
    isPartOfABundle = false;
    if ( parseMessage(buffer,len) == 0 ) callback(message);
  }
  
}

int MicroOsc::parseMessage(unsigned char  *buffer, const size_t len) {
  // NOTE(mhroth): if there's a comma in the address, that's weird
  size_t i = 0;
  while (buffer[i] != '\0') ++i; // find the null-terimated address
  while (buffer[i] != ',') ++i; // find the comma which starts the format string
  if (i >= len) return -1; // error while looking for format string
  // format string is null terminated
  message.format = (char*)(buffer + i + 1); // format starts after comma

  while (i < len && buffer[i] != '\0') ++i;
  if (i == len) return -2; // format string not null terminated

  i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
  message.marker = buffer + i;

  message.buffer = buffer;
  message.len = len;

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


void MicroOsc::parseBundle(unsigned char  *buffer, const size_t len) {
  bundle.buffer =  buffer;
  bundle.marker = buffer + 16; // move past '#bundle ' and timetag fields
  bundle.bufLen = len;
  bundle.bundleLen = len;
}





bool MicroOsc::getNextMessage() {
  if ((bundle.marker - bundle.buffer) >= bundle.bundleLen) return false;
  
  uint32_t lenBE = *((uint32_t *) bundle.marker);
  uint32_t len = uOsc_bigEndian(lenBE);
  
  parseMessage(bundle.marker+4, len);
  bundle.marker += (4 + len); // move marker to next bundle element
  return true;
}




MicroOscMessage::MicroOscMessage() {

}

int32_t MicroOscMessage::getNextInt32() {
  // convert from big-endian (network btye order)
  const int32_t iBE = *((int32_t *) marker);
  const int32_t i  = uOsc_bigEndian(iBE);
  
  marker += 4;
  return i;
}


float MicroOscMessage::getNextFloat() {
  // convert from big-endian (network btye order)
  const uint32_t iBE = *((uint32_t *) marker);
  const uint32_t i  = uOsc_bigEndian(iBE);
  marker += 4;
  return *((float *) (&i)); // HARD CAST TO FLOAT
}


const char* MicroOscMessage::getNextString() {
  int i = (int) strlen((const char*)marker);
  if (marker + i >= buffer + len) return NULL;
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

void MicroOscMessage::getNextBlob( const unsigned char  **blob, uint32_t *bloblen) {

  const uint32_t iBE = *((uint32_t *) marker);
  uint32_t i  = uOsc_bigEndian(iBE);
  
  if (marker + 4 + i <= buffer + len) {
    *bloblen = i; // length of blob
    *blob = marker + 4;
    i = (i + 7) & ~0x3;
    marker += i;
  } else {
    *bloblen = 0;
    *blob = NULL;
  }
}

void MicroOscMessage::getNextMidi( const unsigned char  **midiData) {

  if (marker + 4  <= buffer + len) {
    *midiData = marker;
     marker += 4;
  } else {
    *midiData = NULL;
  }
}


/*
size_t MicroOscMessage::getNextBlob( const unsigned char  **blob, size_t maxLength) {
  size_t i = (size_t) ntohl(*((uint32_t *) message.marker)); // get the blob length
  if (i <= maxLength &&  message.marker + 4 + i <= message.buffer + message.len) {
    size_t len = i; // length of blob
    *blob = message.marker + 4;
    i = (i + 7) & ~0x3;
    message.marker += i;
    return len;
  } else {
    
    *blob = NULL;
    return 0;
  }
}
*/