#pragma once

#include <Arduino.h>
#include <MeshCore.h>
#include <variant.h>

#if defined(ESP_PLATFORM)

#include <helpers/ESP32Board.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class TinyLora_C3_V3_22S_GPS_Board : public ESP32Board {
  static void heartbeatTask(void* param) {
    while (true) {
      digitalWrite(PIN_LED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(50));
      digitalWrite(PIN_LED, LOW);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

public:
  void begin() {
    ESP32Board::begin();

    // GPS enable pin
    pinMode(PIN_GPS_EN, OUTPUT);
    digitalWrite(PIN_GPS_EN, GPS_EN_ACTIVE ? HIGH : LOW);

    pinMode(PIN_LED, OUTPUT);
    for (int i = 4; i >= 0; i--) {
      digitalWrite(PIN_LED, HIGH);
      delay(200);
      digitalWrite(PIN_LED, LOW);
    }
    delay(500);
    xTaskCreate(heartbeatTask, "led_hb", 2048, this, 1, NULL);
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

  const char *getManufacturerName() const override { return "TinyLora C3 V3 22S GPS"; }
};

#endif
