/* MicroOsc
 * By Thomas O Fredericks (tof@tofstuff.com) 
 * Inspired by TinyOsc https://github.com/mhroth/TinyOsc by Martin Roth (mhroth@gmail.com)
 */

#ifndef _MICRO_OSC_
#define _MICRO_OSC_

#include <stdbool.h>
#include <stdint.h>

#include "Print.h"


//#define MICRO_OSC_DEBUG

typedef struct uOscBundle {
  unsigned char* marker; // the current write head (where the next message will be written)
  unsigned char* buffer; // the original buffer
  size_t bufLen; // the byte length of the original buffer
  size_t bundleLen; // the byte length of the total bundle
} uOscBundle;


class MicroOscMessage {
    

	public:

	char* format;  // a pointer to the format field
	unsigned char* marker;  // the current read head
	unsigned char* buffer;  // the original message data (also points to the address)
	size_t len;  // length of the buffer data



	MicroOscMessage();

	/**
	 * Returns true if the address matches.
	 */
	bool fullMatch(const char* address);

	/**
	 * Returns true if the address and type tag matches.
	 */
	bool fullMatch(const char* address, const char * typetags);


	/**
	 * Returns the next 32-bit int. Does not check buffer bounds.
	 */
	int32_t getNextInt32();


	/**
	 * Returns the next 32-bit float. Does not check buffer bounds.
	 */
	float getNextFloat();


	/**
	 * Returns the next string, or NULL if the buffer length is exceeded.
	 */
	const char* getNextString();

	/**
	 * Returns a pointer to the blob data. Returns NULL if the OSC buffer bounds are exceeded.
	 * Sets the value of the len pointer to the length of the blob. Set to 0 if the OSC buffer bounds are exceeded.
	 */
	void getNextBlob(const uint8_t **blobData, uint32_t *len);

	
	/**
	 * Returns a pointer to the MIDI data. Returns NULL if the OSC buffer bounds are exceeded.
	 * MIDI data always has a length of 4.
	 */
	void getNextMidi(const uint8_t **midiData);

	

};


class MicroOsc
{
	 public:
	 	typedef void (*tOscCallbackFunction)(MicroOscMessage&);

/*!
    @brief  Create an instance of the MicroOsc class.

    @code

    // Create an instance of the MicroOsc class.
    MicroOsc() osc;

    @endcode
*/
    MicroOsc();

 /**
 * Parse a buffer containing an OSC message or OSC bundle.
 * The contents of the buffer are NOT copied.
 * Calls the callback for every message received in a bundle or not.
 */
void parseMessages(tOscCallbackFunction callback , unsigned char *buffer, const size_t len);



 private:
     	unsigned char *buffer;
     	//int len;
     	uOscBundle bundle;
     	//uOscBundle* b;
     	//tosc_message* o;
     	MicroOscMessage message;
     	tOscCallbackFunction callback;
     	uint64_t timetag;
     	bool isPartOfABundle;
		uint8_t nullChar = '\0';


uint64_t parseBundleTimeTag();

void parseBundle(unsigned char  *buffer, const size_t len);

 /**
 * Parse a buffer containing an OSC message.
 * The contents of the buffer are NOT copied.
 * Returns 0 if there is no error. An error code (a negative number) otherwise.
 */
int parseMessage(unsigned char  *buffer, const size_t len);



/**
 * Returns true if the message is a bundle. False otherwise.
 */
bool isABundle(const unsigned char  *buffer);


/**
 * Parses the next message in a bundle. Returns true if successful.
 * False otherwise.
 */
bool getNextMessage();

protected :
inline size_t padTheSize(Print* output, size_t i);
size_t writeAddress(Print* output, const char *address);
size_t writeFormat(Print* output, const char *format);
size_t writeInt(Print* output, int32_t i);
size_t vprint(Print* output, const char *address, const char *format, va_list ap);



/**
 * Writes an OSC packet to a buffer. Returns the total number of bytes written.
 * The entire buffer is cleared before writing.
 */
/*
size_t writeMessage(unsigned char *buffer, const size_t len, const char *address,
    const char *fmt, ...);


};

*/
/**
 * Writes an OSC packet to a buffer. Returns the total number of bytes written.
 * The entire buffer is cleared before writing.
 */
void writeMessage(Print *output, const char *address, const char *fmt, ...);


};



#endif // _MICRO_OSC_
