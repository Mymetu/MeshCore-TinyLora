#pragma once

#include <RadioLib.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#include <helpers/radiolib/SX126xReset.h>

// Auto-detect wrapper for the TinyLora C3 V4/V5 board.
//
// The board may be populated with any of three pin-compatible SX126x-family
// chips: SX1262, SX1268 or LLCC68. The chip is identified at boot by the
// RadioLib version-string check (register 0x0150, see SX126x::findChip).
// This wrapper dispatches every radio call through the common SX126x base
// class (all three chips share the SX126x command set; LLCC68 only adds
// tighter SF/BW limits which RadioLib enforces in its virtual overrides),
// so one fixed object can be re-pointed at whichever chip was detected.

#ifndef SX126X_IRQ_HEADER_VALID
#define SX126X_IRQ_HEADER_VALID                0b0000010000  //  4     4     valid LoRa header received
#endif
#ifndef SX126X_IRQ_PREAMBLE_DETECTED
#define SX126X_IRQ_PREAMBLE_DETECTED           0x04
#endif

class CustomSX126xAutoWrapper : public RadioLibWrapper {
  public:
    CustomSX126xAutoWrapper(SX126x& radio, mesh::MainBoard& board) : RadioLibWrapper(radio, board) { }

    // Re-point the wrapper at the chip detected by radio_init().
    void select(SX126x& radio) { _radio = &radio; }

    void setParams(float freq, float bw, uint8_t sf, uint8_t cr) override {
      ((SX126x *)_radio)->setFrequency(freq);
      ((SX126x *)_radio)->setSpreadingFactor(sf);
      ((SX126x *)_radio)->setBandwidth(bw);
      ((SX126x *)_radio)->setCodingRate(cr);
      updatePreamble(sf);
    }

    bool isReceivingPacket() override {
      uint16_t irq = ((SX126x *)_radio)->getIrqFlags();
      return (irq & SX126X_IRQ_HEADER_VALID) || (irq & SX126X_IRQ_PREAMBLE_DETECTED);
    }

    float getCurrentRSSI() override {
      return ((SX126x *)_radio)->getRSSI(false);
    }
    float getLastRSSI() const override { return ((SX126x *)_radio)->getRSSI(); }
    float getLastSNR() const override { return ((SX126x *)_radio)->getSNR(); }

    float packetScore(float snr, int packet_len) override {
      int sf = ((SX126x *)_radio)->spreadingFactor;
      return packetScoreInt(snr, sf, packet_len);
    }
    uint8_t getSpreadingFactor() const override { return ((SX126x *)_radio)->spreadingFactor; }

    void doResetAGC() override { sx126xResetAGC((SX126x *)_radio); }

    void setRxBoostedGainMode(bool en) override {
      ((SX126x *)_radio)->setRxBoostedGainMode(en);
    }
    bool getRxBoostedGainMode() const override {
      uint8_t rxGain = 0;
      ((SX126x *)_radio)->readRegister(RADIOLIB_SX126X_REG_RX_GAIN, &rxGain, 1);
      return (rxGain == RADIOLIB_SX126X_RX_GAIN_BOOSTED);
    }
};
