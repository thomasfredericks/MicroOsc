/* MicroOscSlip
 * By Thomas O Fredericks (tof@tofstuff.com) 
 */

#ifndef _MICRO_OSC_SLIP_
#define _MICRO_OSC_SLIP_

#include <MicroOsc.h>


// Internal-> SLIP reserved codes->
// 192, 219, 220, 221
//#define SLIP_END     0xC0
//#define SLIP_ESC     0xDB
//#define SLIP_ESC_END 0xDC
//#define SLIP_ESC_ESC 0xDD



class MicroSlip : public Print  {



 private:

 	const uint8_t SLIP_END = 192;
	const uint8_t SLIP_ESC = 219;
	const uint8_t SLIP_ESC_END = 220;
	const uint8_t SLIP_ESC_ESC = 221;
	
	size_t length =0;
	size_t parseIndex=0;
	bool error = false;
	bool escaping =false;
    Stream* stream;


	public:

	MicroSlip(Stream* stream) {
		this->stream = stream;

	}

	// THIS COULD BE OPTIMISED
	size_t  parsePacket(uint8_t *buffer, size_t bufferSize) {
			 while ( stream->available()>0 ) {
		      int streamByte = stream->read();

		
		      // END OF MESSAGE, RETURN NUMBER OF BYTES
		      if ( streamByte == SLIP_END ) {

		        length = parseIndex;
		        parseIndex = 0;
		        error = false;

		        return length;
		        // MESSAGE DATA
		      } else if ( error == false ) {
		        // ESCAPING
		        if ( escaping ) {
		        	if ( streamByte ==  SLIP_ESC_END ) {
		        		buffer[parseIndex++]=SLIP_END; 
		        	} else if ( streamByte ==  SLIP_ESC_ESC ) {
		        		buffer[parseIndex++]=SLIP_ESC;
		        	}
/*
		          switch (streamByte) {
		          case SLIP_ESC_END:
		            buffer[parseIndex++]=SLIP_END; 
		            break;
		          case SLIP_ESC_ESC:
		            buffer[parseIndex++]=(SLIP_ESC);
		            break;
		          }
*/
		          escaping = false;
		        } else {
		          // NON ESCAPING
		          if ( streamByte == SLIP_ESC ) {
		            escaping = true;
		          } else {
		            buffer[parseIndex++]=(streamByte);
		          }
		        }
		        // ERROR
		        if ( parseIndex >= bufferSize ) {
		        parseIndex =0;
		          error = true;
		        }
		      }
		    }

    return 0;
		}
  
  // virtual size_t write(uint8_t) = 0;
	virtual size_t  write(uint8_t value) {

		if ( value == 192) {
				stream->write(SLIP_ESC);
				stream->write(SLIP_ESC_END);
				return 2;
		} else if ( value == SLIP_ESC ) {
			  stream->write(SLIP_ESC);
				stream->write(SLIP_ESC_ESC);
				return 2;
		} else {
			stream->write(value);
			return 1;
		}

				
	}

	virtual void beginPacket() {
		stream->write(SLIP_END);
	}
    
    virtual void endPacket() {
    	stream->write(SLIP_END);
    }


	virtual uint8_t  write (uint8_t  *outputBuffer, size_t outputBufferLength) {
			while (outputBufferLength--) {
				uint8_t  value = *outputBuffer++;
				write(value);
			}
			return 1;
	}


};

template <const size_t MICRO_OSC_IN_SIZE>
class MicroOscSlip : public MicroOsc {

    MicroSlip slip;
    unsigned char inputBuffer[MICRO_OSC_IN_SIZE];
	
	protected:
	void beginMessage() {
		slip.beginPacket();
	}
	void endMessage() {
		slip.endPacket(); 
	}

  public:
    MicroOscSlip(Stream * stream) : slip(stream), MicroOsc(&slip) {

    }


    virtual void onOscMessageReceived(tOscCallbackFunction callback) {

      size_t packetLength = slip.parsePacket(inputBuffer, MICRO_OSC_IN_SIZE );
      if ( packetLength > 0 ) {
        MicroOsc::parseMessages( callback , inputBuffer , packetLength );
      }
    }
   
    [[deprecated("Use onOscMessageReceived(callback) instead.")]]
    void receiveMessages(tOscCallbackFunction callback) {
        onOscMessageReceived(callback);
    }


};

#endif // _MICRO_OSC_SLIP_

