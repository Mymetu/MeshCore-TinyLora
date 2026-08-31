#include <Arduino.h>
#include "target.h"

TinyLora_C3_V3_22S_GPS_Board board;

// The V3 22S GPS board can be populated with SX1262, SX1268 or LLCC68 (same
// pinout). One Module instance is shared by all three candidate radios;
// radio_init() identifies the chip at boot (RadioLib version-string check)
// and re-points the single wrapper at the detected radio.
Module radio_module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY);
CustomSX1262 radio_sx1262(&radio_module);
CustomSX1268 radio_sx1268(&radio_module);
CustomLLCC68 radio_llcc68(&radio_module);

// Fixed global object - MyMesh etc. bind references to it at startup, so it
// must exist before radio_init() runs. It is re-pointed by select() once the
// chip has been detected (before any radio method is called).
CustomSX126xAutoWrapper radio_driver(radio_sx1262, board);

PhysicalLayer* g_radio = nullptr;
const char* g_radio_chip_name = nullptr;

ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);

#if ENV_INCLUDE_GPS
  #include <helpers/sensors/MicroNMEALocationProvider.h>
  MicroNMEALocationProvider nmea = MicroNMEALocationProvider(Serial1, &rtc_clock);
  EnvironmentSensorManager sensors = EnvironmentSensorManager(nmea);
#else
  EnvironmentSensorManager sensors;
#endif

bool radio_init() {
  fallback_clock.begin();
  rtc_clock.begin(Wire);

#ifdef FORCE_RADIO_SX1268
  Serial.println("Radio: forced SX1268");
  if (radio_sx1268.std_init(&SPI)) {
    g_radio = &radio_sx1268;
    g_radio_chip_name = "SX1268";
    radio_driver.select(radio_sx1268);
    Serial.println("Radio: SX1268 OK");
    return true;
  }
  Serial.println("ERROR: forced SX1268 init failed");
  return false;
#else
  Serial.println("Radio: detecting chip (SX1262 / SX1268 / LLCC68)...");
  if (radio_sx1262.std_init(&SPI)) {
    g_radio = &radio_sx1262;
    g_radio_chip_name = "SX1262";
    radio_driver.select(radio_sx1262);
    Serial.println("Radio: SX1262 detected");
    return true;
  }
  if (radio_sx1268.std_init(&SPI)) {
    g_radio = &radio_sx1268;
    g_radio_chip_name = "SX1268";
    radio_driver.select(radio_sx1268);
    Serial.println("Radio: SX1268 detected");
    return true;
  }
  if (radio_llcc68.std_init(&SPI)) {
    g_radio = &radio_llcc68;
    g_radio_chip_name = "LLCC68";
    radio_driver.select(radio_llcc68);
    Serial.println("Radio: LLCC68 detected");
    return true;
  }
  Serial.println("ERROR: no known LoRa chip found (expected SX1262 / SX1268 / LLCC68)!");
  return false;
#endif
}

uint32_t radio_get_rng_seed() {
  return g_radio->random(0x7FFFFFFF);
}

void radio_set_params(float freq, float bw, uint8_t sf, uint8_t cr) {
  radio_driver.setParams(freq, bw, sf, cr);
}

void radio_set_tx_power(int8_t dbm) {
  radio_driver.setTxPower(dbm);
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(*g_radio);
  return mesh::LocalIdentity(&rng);
}
