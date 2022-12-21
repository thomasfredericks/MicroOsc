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



class MicroOscMessage {


public:

	char* format;  // a pointer to the format field
	unsigned char* marker;  // the current read head
	unsigned char* buffer;  // the original message data (also points to the address)
	uint32_t bufferLength;  // length of the buffer data



	MicroOscMessage();

	/**
	* Returns `true` if the address matches exactly
	*/
	bool checkOscAddress(const char* address);

	/**
	* Returns `true` if the address and argument type tags match exactly.
	*/
	bool checkOscAddress(const char* address, const char * typetags);

	/**
	* Returns `true` if the address matches exactly
	*/
	[[deprecated("Use checkOscAddress(address) instead.")]]
	bool fullMatch(const char* address);

	/**
	* Returns true if the address and argument type tags match exactly.
	*/
	[[deprecated("Use checkOscAddress(address,typetags) instead.")]]
	bool fullMatch(const char* address, const char * typetags);


	/**
	 * Returns the next argument as a 32-bit int.
	 * Does not check buffer bounds.
	 */
	int32_t nextAsInt();


	/**
	 * Returns the next argument as a 32-bit float.
	 * Does not check buffer bounds.
	 */
	float nextAsFloat();


	/**
	* Treats the next argument as a C string and returns a pointer to the data,
	* or NULL if the buffer length is exceeded.
	*/
	const char* nextAsString();

	/**
	 * Treats the next argument as a blob of data and fills a pointer with the address to a byte array.
	* The pointer is NULL if there was an error.
	 * Returns the length of the byte blob. Returns 0 if there was an error.
	 */
	uint32_t nextAsBlob(const uint8_t **blobData);


	/**
	 * Treats the next value as MIDI and fills a pointer with the address to the MIDI data.
	* The pointer is NULL if the OSC bounds are exceeded.
	 * MIDI data always has a length of 4. Bytes from MSB to LSB are: port id, status byte, data1, data2
	 */
	int nextAsMidi(const uint8_t **midiData);



};


class MicroOsc
{

public:
	typedef void (*tOscCallbackFunction)(MicroOscMessage&);





private:
	struct uOscBundle {
		unsigned char* marker; // the current write head (where the next message will be written)
		unsigned char* buffer; // the original buffer
		size_t bufLen; // the byte length of the original buffer
		size_t bundleLen; // the byte length of the total bundle
	} ;

	//unsigned char *buffer;
	//int len;
	struct uOscBundle bundle;
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
	virtual void beginMessage() = 0;
	virtual void endMessage() = 0;


public:
	/**
	* Check for messages and execute callback for every received message
	*/
	virtual void onOscMessageReceived(tOscCallbackFunction callback) = 0;

	/**
	 * Send an OSC message with any mnumber of arguments of diffrent types
	 */
	void sendMessage(const char *address, const char *format, ...);
	/**
	* Send a single int OSC message
	*/
	void sendInt(const char *address, int32_t i);
	/**
	* Send a single float OSC message
	*/
	void sendFloat(const char *address, float f);
	/**
	* Send a single string OSC message
	*/
	void sendString(const char *address, const char *str);
	/**
	* Send a single blob (array of bytes) OSC message
	*/
	void sendBlob(const char *address, unsigned char *b, int32_t length);
	/**
	* Send a single double OSC message
	*/
	void sendDouble(const char *address, double d);
	/**
	* Send a single MIDI OSC message
	*/
	void sendMidi(const char *address, unsigned char *midi);
	/**
	* Send a single Int64 OSC message
	*/
	void sendInt64(const char *address, uint64_t h);
};



#endif // _MICRO_OSC_
