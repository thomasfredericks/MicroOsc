#ifndef _MICRO_OSC_MESSAGE_
#define _MICRO_OSC_MESSAGE_

#include <Arduino.h>

class MicroOsc; // FORWARD DECLARATION;

class MicroOscMessage
{

	friend MicroOsc;

	union IntFloatUnion
	{
		uint32_t int_value_;
		float float_value_;
	};

	union IntDoubleUnion
	{
		uint64_t int_value_;
		double double_value_;
	};

protected:
	
	char *format_;			 // a pointer to the format field
	char *format_marker_;	 // the current format read head
	unsigned char *marker_;	 // the current read head
	unsigned char *buffer_;	 // the original message data (also points to the address)
	uint32_t buffer_length_; // length of the buffer data

private:
	void advance(uint32_t bytes)
	{
		marker_ += bytes;
		format_marker_++;
	}

public:
	MicroOscMessage();

	/**
	 * Parses an OSC message from a buffer.
	 * Returns 0 if there is no error. An error code (a negative number) otherwise.
	 * The contents of the buffer is NOT copied.
	 */
	int parseMessage(unsigned char  *buffer, const size_t bufferLength);

	/**
	 * Returns the current type tag without advancing the read head
	 */
	char getTypeTag()
	{
		return *format_marker_;
	}

	/**
	 * Returns the OSC address of the message
	 * The returned value is valid only until the next received message. DO NOT STORE IT.
	 */
	const char * getOscAddress(const char *address);

	/**
	 * Returns `true` if the address matches exactly
	 */
	bool checkOscAddress(const char *address);

	/**
	 * Returns `true` if the address and argument type tags match exactly.
	 */
	bool checkOscAddressAndTypeTags(const char *address, const char *typetags);



	/**
	 * Returns `true` if the address and argument type tags match exactly.
	 */
	[[deprecated("Use checkOscAddressAndType(address) instead.")]]
	bool checkOscAddress(const char *address, const char *typetags)
	{
		return checkOscAddressAndTypeTags(address, typetags);
	};

	/**
	 * Copies the address into a `char*` destinationBuffer of maximum length destinationBufferMaxLength.
	 */
	void copyAddress(char *destinationBuffer, size_t destinationBufferMaxLength);

	/**
	 * Copies the type tags into a `char*` destinationBuffer of maximum length destinationBufferMaxLength.
	 */
	void copyTypeTags(char *destinationBuffer, size_t destinationBufferMaxLength);

	/**
	 * Returns `true` if the address matches exactly
	 */
	[[deprecated("Use checkOscAddress(address) instead.")]]
	bool fullMatch(const char *address)
	{
		return checkOscAddress(address);
	};

	/**
	 * Returns true if the address and argument type tags match exactly.
	 */
	[[deprecated("Use checkOscAddressAndTypeTags(address,typetags) instead.")]]
	bool fullMatch(const char *address, const char *typetags)
	{
		return checkOscAddressAndTypeTags(address, typetags);
	};

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
	 * Returns the next argument as a 64-bit double.
	 * Does not check buffer bounds.
	 */
	double nextAsDouble();

	/**
	 * Treats the next argument as a C string and returns a pointer to the data,
	 * or NULL if the buffer length is exceeded.
	 */
	const char *nextAsString();

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

#endif