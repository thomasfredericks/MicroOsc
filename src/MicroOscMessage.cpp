#include "MicroOscMessage.h"
#include "MicroOscUtility.h"

MicroOscMessage::MicroOscMessage()
{
}

int32_t MicroOscMessage::nextAsInt()
{
  // convert from big-endian (network btye order)
  const int32_t iBE = *((int32_t *)marker_);
  const int32_t i = uOsc_bigEndian(iBE);
  // marker += 4;
  advance(4);
  return i;
}

int MicroOscMessage::parseMessage(unsigned char *buffer, const size_t bufferLength)
{
  // NOTE(mhroth): if there's a comma in the address, that's weird
  size_t i = 0;
  while (buffer[i] != '\0')
    ++i; // find the null-terimated address
  while (buffer[i] != ',')
    ++i; // find the comma which starts the format string
  if (i >= bufferLength)
    return -1; // error while looking for format string
  // format string is null terminated
  format_ = (char *)(buffer + i + 1); // format starts after comma
  format_marker_ = format_;

  while (i < bufferLength && buffer[i] != '\0')
    ++i;
  if (i == bufferLength)
    return -2; // format string not null terminated

  i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
  marker_ = buffer + i;

  buffer_ = buffer;
  buffer_length_ = bufferLength;



  return 0;
}

float MicroOscMessage::nextAsFloat()
{
  // convert from big-endian (network btye order)
  const uint32_t iBE = *((uint32_t *)marker_);
  // marker += 4;
  advance(4);
  /*
    const uint32_t i  = uOsc_bigEndian(iBE);
    return *((float *) (&i)); // HARD CAST TO FLOAT
    */
  union IntFloatUnion u;
  u.int_value_ = uOsc_bigEndian(iBE);

  return u.float_value_;
}

double MicroOscMessage::nextAsDouble()
{
  // convert from big-endian (network byte order)
  const uint64_t iBE = *((uint64_t *)marker_);
  // marker += 8;
  advance(8);

  union IntDoubleUnion u;
  u.int_value_ = uOsc_bigEndian(iBE);

  return u.double_value_;
}

const char *MicroOscMessage::nextAsString()
{
  int i = (int)strlen((const char *)marker_);
  if (marker_ + i >= buffer_ + buffer_length_)
    return NULL;
  const char *s = (const char *)marker_;
  i = (i + 4) & ~0x3; // advance to next multiple of 4 after trailing '\0'
  // marker_ += i;
  advance(i);
  return s;
}

const char *MicroOscMessage::getOscAddress()
{
  return (const char *)buffer_;
}

void MicroOscMessage::copyAddress(char *destinationBuffer, size_t destinationBufferMaxLength)
{
  strncpy(destinationBuffer, (const char *)buffer_, destinationBufferMaxLength);
}

void MicroOscMessage::copyTypeTags(char *destinationBuffer, size_t destinationBufferMaxLength)
{
  strncpy(destinationBuffer, (const char *)format_, destinationBufferMaxLength);
}

bool MicroOscMessage::checkOscAddress(const char *address)
{
  return (strcmp((const char *)buffer_, address) == 0);
}

bool MicroOscMessage::checkOscAddressAndTypeTags(const char *address, const char *typetags)
{
  return (strcmp((const char *)buffer_, address) == 0) && (strcmp((const char *)format_, typetags) == 0);
}

uint32_t MicroOscMessage::nextAsBlob(const unsigned char **blob)
{

  const uint32_t iBE = *((uint32_t *)marker_);

  uint32_t length = 0;
  uint32_t i = uOsc_bigEndian(iBE);

  if (marker_ + 4 + i <= buffer_ + buffer_length_)
  {             // not bigger than stored data
    length = i; // length of blob
    *blob = marker_ + 4;
    i = (i + 7) & ~0x3;
    // marker_ += i;
    advance(i);
  }
  else
  {
    length = 0;
    *blob = NULL;
  }

  return length;
}

int MicroOscMessage::nextAsMidi(const unsigned char **midiData)
{

  if (marker_ + 4 <= buffer_ + buffer_length_)
  {
    *midiData = marker_;
    // marker += 4;
    advance(4);
    return 4;
  }
  else
  {
    return 0;
  }
}
