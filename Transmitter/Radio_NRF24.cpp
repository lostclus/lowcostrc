#include <string.h>
#include <Arduino.h>
#include <LowcostRC_Console.h>
#include "Config.h"
#include "Radio_NRF24.h"

#define NRF24_DEFAULT_CHANNEL 76

NRF24RadioModule::NRF24RadioModule()
  : rf24(RADIO_NRF24_CE_PIN, RADIO_NRF24_CSN_PIN)
{
}

bool NRF24RadioModule::begin() {
  if (!rf24.begin()) {
    PRINTLN(F("NRF24: init: FAIL"));
    return false;
  }

  PRINTLN(F("NRF24: init: OK"));
  rf24.setRadiation(RF24_PA_MAX, RF24_250KBPS);
  rf24.setPayloadSize(PACKET_SIZE);
  rf24.enableAckPayload();
  return true;
}

uint8_t NRF24RadioModule::rfChannelToNRF24(RFChannel ch) {
  return (ch == 0) ? NRF24_DEFAULT_CHANNEL : ch;
}

bool NRF24RadioModule::setPeer(const Address *addr) {
  memcpy(&peer, addr, sizeof(peer));
  rf24.openWritingPipe(peer.address);
  return true;
}

bool NRF24RadioModule::setRFChannel(RFChannel ch) {
  rfChannel = ch;
  rf24.setChannel(rfChannelToNRF24(rfChannel));
  return true;
}

bool NRF24RadioModule::receive(union ResponsePacket *packet) {
  if (rf24.isAckPayloadAvailable()) {
    rf24.read(packet, sizeof(ResponsePacket));
    return true;
  }
  return false;
}

bool NRF24RadioModule::send(const union RequestPacket *packet) {
    return rf24.write(packet, sizeof(RequestPacket));
}

bool NRF24RadioModule::pair() {
  Address broadcast = ADDRESS_BROADCAST;
  RequestPacket req, resp;

  rf24.setChannel(NRF24_DEFAULT_CHANNEL);
  rf24.openWritingPipe(broadcast.address);

  req.pair.packetType = PACKET_TYPE_PAIR;
  req.pair.session = random(1 << 15);
  req.pair.status = PAIR_STATUS_INIT;

  PRINT(F("NRF24: Starting pairing session: "));
  PRINTLN(req.pair.session);

  for (
    unsigned long start = millis();
    millis() - start < 3000;
    delay(5)
  ) {
    rf24.write(&req, sizeof(req));
    if (rf24.isAckPayloadAvailable()) {
      rf24.read(&resp, sizeof(resp));
      if (
          resp.pair.packetType == PACKET_TYPE_PAIR
          && resp.pair.session == req.pair.session
          && resp.pair.status == PAIR_STATUS_READY
      ) {
          PRINTLN(F("NRF24: Paired"));
          memcpy(&peer, &resp.pair.sender, sizeof(peer));
          req.pair.status = PAIR_STATUS_PAIRED;
          rf24.write(&req, sizeof(req));
          rf24.openWritingPipe(peer.address);
          rf24.setChannel(NRF24_DEFAULT_CHANNEL);
          return true;
      }
    }
  }

  PRINTLN(F("NRF24: Not paired"));
  rf24.openWritingPipe(peer.address);
  rf24.setChannel(rfChannelToNRF24(rfChannel));

  return false;
}

// vim:ai:sw=2:et
