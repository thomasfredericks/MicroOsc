/* MicroOscSlip
 * By Thomas O Fredericks (tof@tofstuff.com) 
 */

#ifndef _MICRO_OSC_SLIP_
#define _MICRO_OSC_SLIP_

#include <MicroOsc.h>
#include <MicroSlip.h>

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
	bool readyToSendMessage() {
		return true;
	}

  public:
    MicroOscSlip(Stream * stream) : MicroOsc(&slip), slip(stream) {

    }

	MicroOscSlip(Stream & stream) : MicroOsc(&slip), slip(&stream) {

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

