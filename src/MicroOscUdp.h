/* MicroOscUdp
 * By Thomas O Fredericks (tof@tofstuff.com) 
 */

//#define MICRO_OSC_DEBUG 

#ifndef _MICRO_OSC_UDP_
#define _MICRO_OSC_UDP_

#include <MicroOsc.h>
#include <Udp.h>


template <const size_t MICRO_OSC_IN_SIZE>
class MicroOscUdp : public MicroOsc {

    UDP* udp;
    unsigned char inputBuffer[MICRO_OSC_IN_SIZE];
    IPAddress destinationIp = INADDR_NONE;
    unsigned int destinationPort;

    protected:
	virtual void beginMessage() {
    /*
    Serial.print("Begin UDP OSC IP: ");
    Serial.print(destinationIp);
    Serial.print(". Port: ");
    Serial.println(destinationPort);
    */
		udp->beginPacket(destinationIp, destinationPort);
	}
	virtual void endMessage() {
    /*
    Serial.println("End UDP OSC");
    */
		 udp->endPacket(); 
	}

  bool readyToSendMessage() {
    return destinationIp != INADDR_NONE;
  }

  public:
    MicroOscUdp(UDP * udp, IPAddress destinationIp, unsigned int destinationPort) : MicroOsc(udp) {
    	this->udp = udp;
		  this->destinationIp = destinationIp;
		  this->destinationPort = destinationPort;
    }
     
     MicroOscUdp(UDP * udp) : MicroOsc(udp) {
      this->udp = udp;
      this->destinationIp = INADDR_NONE;
    }

     MicroOscUdp(UDP & udp, IPAddress destinationIp, unsigned int destinationPort) : MicroOsc(&udp) {
    	this->udp = &udp;
		  this->destinationIp = destinationIp;
		  this->destinationPort = destinationPort;
    }
     
     MicroOscUdp(UDP & udp) : MicroOsc(&udp) {
      this->udp = &udp;
      this->destinationIp = INADDR_NONE;
    }


    virtual void onOscMessageReceived(tOscCallbackFunction callback) {
      size_t packetLength = udp->parsePacket();
      if ( packetLength > 0 ) {
        packetLength = udp->read(inputBuffer, MICRO_OSC_IN_SIZE);
		
        #ifdef MICRO_OSC_DEBUG    
        Serial.print("MICRO_OSC_DEBUG Received UDP bytes: ");
        Serial.println(packetLength);
        #endif
      	
        MicroOsc::parseMessages( callback , inputBuffer , packetLength);
      }
    }

    [[deprecated("Use onOscMessageReceived(callback) instead.")]]
    void receiveMessages(tOscCallbackFunction callback) {
        onOscMessageReceived(callback);
    }

    void setDestination(IPAddress destinationIp, unsigned int destinationPort) {
      this->destinationIp = destinationIp;
      this->destinationPort = destinationPort;
    }

};


#endif // _MICRO_OSC_UDP_