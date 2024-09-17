#ifndef LOWCOSTRC_RX_NRF24_H
#define LOWCOSTRC_RX_NRF24_H

#include <LowcostRC_Protocol.h>
#include <LowcostRC_Rx.h>
#include <RF24.h>

#define NRF24_DEFAULT_CHANNEL 76
#define NRF24_ADDRESS_LENGTH 5

class NRF24Receiver : public BaseReceiver {
  private:
    RF24 rf24;
    Address address;
    RFChannel rfChannel;

    uint8_t rfChannelToNRF24(RFChannel ch);
    void configure(const Address *addr, RFChannel ch);
  public:

    NRF24Receiver(uint8_t cepin, uint8_t cspin);
    bool begin(const Address *addr, RFChannel ch);

    virtual const Address *getAddress();
    virtual const Address *getPeerAddress();
    virtual RFChannel getRFChannel();
    virtual void setRFChannel(RFChannel ch);
    virtual bool receive(RequestPacket *packet);
    virtual void send(const ResponsePacket *packet);
    virtual bool pair();
    virtual bool isPaired();
};

#endif // LOWCOSTRC_RX_NRF24_H
// vim:et:sw=2:ai
