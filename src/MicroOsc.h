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
	uint32_t bufferLength;  // length of the buffer data



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
	int32_t nextAsInt();


	/**
	 * Returns the next 32-bit float. Does not check buffer bounds.
	 */
	float nextAsFloat();


	/**
	 * Returns the next string, or NULL if the buffer length is exceeded.
	 */
	const char* nextAsString();

	/**
	 * Points to the blob data. The pointer is NULL if there was an error.
	 * Returns the length of the blob. Returns 0 if there was an error.
	 */
	uint32_t nextAsBlob(const uint8_t **blobData);

	
	/**
	 * Returns a pointer to the MIDI data. Returns NULL if the OSC buffer bounds are exceeded.
	 * MIDI data always has a length of 4.
	 */
	int nextAsMidi(const uint8_t **midiData);

	

};


class MicroOsc
{
	 public:
	 	typedef void (*tOscCallbackFunction)(MicroOscMessage&);





 private:
     	//unsigned char *buffer;
     	//int len;
     	uOscBundle bundle;
     	//uOscBundle* b;
     	//tosc_message* o;
     	MicroOscMessage message;
     	tOscCallbackFunction callback;
     	uint64_t timetag;
     	//bool isPartOfABundle;
		const uint8_t nullChar = '\0';
		Print* output;
		uint32_t outputWritten = 0;


		public:
		/*!
    @brief  Create an instance of the MicroOsc class.
*/
    MicroOsc(Print* output);

 /**
 * Parse a buffer containing an OSC message or OSC bundle.
 * The contents of the buffer are NOT copied.
 * Calls the callback for every message received in a bundle or not.
 */
void parseMessages(tOscCallbackFunction callback , unsigned char *buffer, const size_t len);

private:


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
void padTheSize();
void writeAddress(const char *address);
void writeFormat(const char *format);
void writeInt( int32_t i);
void writeFloat(float f);
void writeString(const char *str);
void writeBlob(unsigned char *b, int32_t length);
void writeDouble(double d);
void writeMidi(const unsigned char *midi);
void writeInt64(uint64_t h);


private:
void writeMessage(const char *address, const char *format, va_list ap);
//void vprint(const char *address, const char *format, va_list ap);


protected:
	virtual void beginMessage()=0;
	virtual void endMessage()=0;
	

public:
	void sendMessage(const char *address, const char *format, ...);
	void sendInt(const char *address, int32_t i);
	void sendFloat(const char *address, float f);
	void sendString(const char *address, const char *str);
	void sendBlob(const char *address, unsigned char *b, int32_t length);
	void sendDouble(const char *address,double d);
	void sendMidi(const char *address,unsigned char *midi);
	void sendInt64(const char *address, uint64_t h);


};



#endif // _MICRO_OSC_
