/* MicroOsc
 * By Thomas O Fredericks (tof@tofstuff.com)
 * Inspired by TinyOsc https://github.com/mhroth/TinyOsc by Martin Roth (mhroth@gmail.com)
 */

#ifndef _MICRO_OSC_
#define _MICRO_OSC_

#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "Print.h"
#include "MicroOscMessage.h"

class MicroOsc
{

public:
	typedef void (*MicroOscCallback)(MicroOscMessage &msg);
	typedef void (*MicroOscCallbackWithSource)(MicroOsc &source, MicroOscMessage &msg);

private:
	struct uOscBundle
	{
		unsigned char *marker; // the current write head (where the next message will be written)
		unsigned char *buffer; // the original buffer
		// size_t bufLen; // the byte length of the original buffer
		int32_t bundleLen; // the byte length of the total bundle
	};

	// unsigned char *buffer;
	// int len;
	struct uOscBundle bundle;
	// uOscBundle* b;
	// tosc_message* o;
	MicroOscMessage message;
	//MicroOscCallback callback;
	//MicroOscCallbackWithSource callbackWithSource;
	uint64_t timetag;
	// bool isPartOfABundle;
	const uint8_t nullChar = '\0';
	Print *output;
	uint32_t outputWritten = 0;

public:
	/*!
	@brief  Create an instance of the MicroOsc class.
	*/
	MicroOsc(Print *output);

	/**
	 * Parse a buffer containing an OSC message or OSC bundle.
	 * The contents of the buffer are NOT copied.
	 * Calls the callback for every message received in a bundle or not.
	 */
	void parseMessages(MicroOscCallback callback, unsigned char *buffer, const size_t len);

	void parseMessages(MicroOscCallbackWithSource callback, unsigned char *buffer, const size_t len);

private:
	uint64_t parseBundleTimeTag();

	void parseBundle(unsigned char *buffer, const size_t len);

	/**
	 * Returns true if the message is a bundle. False otherwise.
	 */
	bool isABundle(const unsigned char *buffer);

	/**
	 * Parses the next message in a bundle. Returns true if successful.
	 * False otherwise.
	 */
	bool getNextMessage();

protected:
	void padTheSize();
	void writeAddress(const char *address);
	void writeFormat(const char *format);
	void writeInt(int32_t i);
	void writeFloat(float f);
	void writeString(const char *str);
	void writeBlob(unsigned char *b, int32_t length);
	void writeDouble(double d);
	void writeMidi(const unsigned char *midi);
	void writeInt64(uint64_t h);

private:
	void writeMessage(const char *address, const char *format, va_list ap);
	// void vprint(const char *address, const char *format, va_list ap);

protected:
	virtual void beginMessage() = 0;
	virtual void endMessage() = 0;
	virtual bool readyToSendMessage() = 0;

private:
	void sendWithoutArguments(const char *address, const char *type);

public:
	/**
	 * Check for messages and execute callback for every received message
	 */
	virtual void onOscMessageReceived(MicroOscCallback callback) = 0;

	/**
	 * Check for messages and execute callback for every received message
	 */
	virtual void onOscMessageReceived(MicroOscCallbackWithSource callback) = 0;

	/**
	 * Send an OSC message with any mnumber of arguments of diffrent types
	 */
	void sendMessage(const char *address, const char *format, ...);
	/**
	 * Send an impulse (aka "bang") message without any arguments.
	 */
	void sendImpulse(const char *address);
	/**
	 * Send a TRUE message without any arguments.
	 */
	void sendTrue(const char *address);
	/**
	 * Send an FALSE message without any arguments.
	 */
	void sendFalse(const char *address);
	/**
	 * Send a message without any arguments.
	 */
	void sendNull(const char *address);
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
