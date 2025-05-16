#ifndef _MICRO_OSC_UTILITY_
#define _MICRO_OSC_UTILITY_

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
  while (size-- > 0) {
    *dst++ = *src--;
  }
  return ret;
}
#endif