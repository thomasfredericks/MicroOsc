#include <MicroOsc.h>
#include <Udp.h>



template <const size_t MICRO_OSC_IN_SIZE>
class MicroOscUdp : public MicroOsc {

    UDP* udp;
    unsigned char inputBuffer[MICRO_OSC_IN_SIZE];
    IPAddress destinationIp;
    unsigned int destinationPort;

  public:
    MicroOscUdp(UDP * udp, IPAddress destinationIp, unsigned int destinationPort) {
    	this->udp = udp;
		this->destinationIp = destinationIp;
		this->destinationPort = destinationPort;
    }

    void receiveMessages(tOscCallbackFunction callback) {

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

    void sendMessage(const char *address, const char *format,...) {
      udp->beginPacket(destinationIp, destinationPort);
      va_list ap;
      va_start(ap, format);
      vprint(udp, address, format, ap);
      va_end(ap);
      udp->endPacket(); 
    }

    void setDestination(IPAddress destinationIp, unsigned int destinationPort) {
      this->destinationIp = destinationIp;
      this->destinationPort = destinationPort;
    }



};
