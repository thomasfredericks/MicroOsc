/* MicroOscSlip
 * By Thomas O Fredericks (tof@tofstuff.com)
 */

#ifndef _MICRO_OSC_SLIP_
#define _MICRO_OSC_SLIP_

#include <MicroOsc.h>
#include <MicroSlip.h>

template <const size_t MICRO_OSC_IN_SIZE>
class MicroOscSlip : public MicroOsc
{
protected:
  MicroSlip slip_;
  unsigned char input_buffer_[MICRO_OSC_IN_SIZE];

protected:
  void transportBegin()
  {
    slip_.beginPacket();
  }
  void transportEnd()
  {
    slip_.endPacket();
  }
  bool transportReady()
  {
    return true;
  }

public:
  MicroOscSlip(Stream *stream) : MicroOsc(&slip_), slip_(stream)
  {
  }

  MicroOscSlip(Stream &stream) : MicroOsc(&slip_), slip_(&stream)
  {
  }

  void onOscMessageReceived(MicroOscCallback callback) override
  {

    size_t packetLength = slip_.parsePacket(input_buffer_, MICRO_OSC_IN_SIZE);
    if (packetLength > 0)
    {
      MicroOsc::parseMessages(callback, input_buffer_, packetLength);
    }
  }

  void onOscMessageReceived(MicroOscCallbackWithSource callback) override
  {

    size_t packetLength = slip_.parsePacket(input_buffer_, MICRO_OSC_IN_SIZE);
    if (packetLength > 0)
    {
      MicroOsc::parseMessages(callback, input_buffer_, packetLength);
    }
  }

  [[deprecated("Use onOscMessageReceived(callback) instead.")]]
  void receiveMessages(MicroOscCallback callback)
  {
    onOscMessageReceived(callback);
  }
};

#endif // _MICRO_OSC_SLIP_
