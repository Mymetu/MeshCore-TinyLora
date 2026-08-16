#pragma once

#include <Arduino.h>
#include <MeshCore.h>
#include <variant.h>

#if defined(ESP_PLATFORM)

#include <helpers/ESP32Board.h>

class TinyLora_MV_ZHCN_Board : public ESP32Board {
public:
  void begin() {
    ESP32Board::begin();
#ifdef PIN_MV_BUZZER
    pinMode(PIN_MV_BUZZER, OUTPUT);
    digitalWrite(PIN_MV_BUZZER, LOW);
#endif
  }

  uint16_t getBattMilliVolts() override {
#ifdef PIN_VBAT_READ
    analogReadResolution(ADC_RESOLUTION);
    uint32_t raw = 0;
    for (int i = 0; i < BATTERY_SAMPLES; i++) {
      raw += analogRead(PIN_VBAT_READ);
    }
    raw /= BATTERY_SAMPLES;
    return (raw * ADC_MULTIPLIER * AREF_VOLTAGE) / 4.096;
#else
    return 0;
#endif
  }

  const char *getManufacturerName() const override { return "TinyLora MV ZHCN"; }
};

#endif
