/*
 * TinyLora C3 V3 29S GPS - ESP32-C3 with an externally amplified SX1268 + GPS.
 * Difference from V3_29S: GPS on Serial1 (pins 20/21), GPS_EN on pin 2.
 */

#pragma once

#define TINYLORA_V3_GPS 1

////////////////////////////////////////////////////////////////////////////////
// GPS
#define HAS_GPS                  1
#define GPS_RX_PIN               21
#define GPS_TX_PIN               20
#define PIN_GPS_EN               2
#define GPS_EN_ACTIVE            1

////////////////////////////////////////////////////////////////////////////////
// Battery
#define PIN_VBAT_READ           (1)
#define ADC_MULTIPLIER          (2.0F)
#define ADC_RESOLUTION          (12)
#define AREF_VOLTAGE            (3.0F)
#define BATTERY_SAMPLES         (8)

////////////////////////////////////////////////////////////////////////////////
// Board pins
// I2C not used — pins 20/21 repurposed for GPS Serial1
#define PIN_LED                 (0)
#define PIN_BUTTON              (9)

////////////////////////////////////////////////////////////////////////////////
// LoRa
#define USE_SX1268

#define P_LORA_NSS               (8)
#define P_LORA_RESET             (5)
#define P_LORA_DIO_0             (RADIOLIB_NC)
#define P_LORA_DIO_1             (3)
#define P_LORA_DIO_2             (RADIOLIB_NC)
#define P_LORA_BUSY              (4)
#define P_LORA_SCLK              (10)
#define P_LORA_MISO              (6)
#define P_LORA_MOSI              (7)
#define SX126X_DIO3_TCXO_VOLTAGE (1.8)
#define SX126X_DIO2_AS_RF_SWITCH (true)
#define SX126X_CURRENT_LIMIT      (140)
#define SX126X_RX_BOOSTED_GAIN    (1)
