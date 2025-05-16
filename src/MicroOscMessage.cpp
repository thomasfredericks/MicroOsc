#include "MicroOscMessage.h"
#include "MicroOscUtility.h"

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
  marker += 4;
  /*
    const uint32_t i  = uOsc_bigEndian(iBE);
    return *((float *) (&i)); // HARD CAST TO FLOAT
    */
  union IntFloatUnion u;
  u.intValue = uOsc_bigEndian(iBE);

  return u.floatValue;
}


const char* MicroOscMessage::nextAsString() {
  int i = (int) strlen((const char*)marker);
  if (marker + i >= buffer + bufferLength) return NULL;
  const char *s = (const char*)marker;
  i = (i + 4) & ~0x3; // advance to next multiple of 4 after trailing '\0'
  marker += i;
  return s;
}

void MicroOscMessage::copyAddress(char * destinationBuffer, size_t destinationBufferMaxLength) {
  strncpy(destinationBuffer, (const char *) buffer, destinationBufferMaxLength);
}

void MicroOscMessage::copyTypeTags(char * destinationBuffer, size_t destinationBufferMaxLength) {
  strncpy(destinationBuffer, (const char *) format, destinationBufferMaxLength);
}



bool MicroOscMessage::checkOscAddress(const char* address) {
  return (strcmp( (const char *) buffer, address) == 0);
}


bool MicroOscMessage::checkOscAddressAndTypeTags(const char* address, const char * typetags) {
  return (strcmp( (const char*) buffer, address) == 0) && (strcmp( (const char*) format, typetags) == 0) ;
}

/*
bool MicroOscMessage::fullMatch(const char* address) {
  return checkOscAddress(address);
}
*/
/*
bool MicroOscMessage::fullMatch(const char* address, const char * typetags) {
  return checkOscAddress(address, typetags);
}
*/
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