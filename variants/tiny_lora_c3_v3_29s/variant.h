/*
 * TinyLora C3 V3 29S - ESP32-C3 with an externally amplified SX1268.
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Battery
#define PIN_VBAT_READ           (1)
#define ADC_MULTIPLIER          (2.0F)
#define ADC_RESOLUTION          (12)
#define AREF_VOLTAGE            (3.0F)
#define BATTERY_SAMPLES         (8)

////////////////////////////////////////////////////////////////////////////////
// Board pins
#define PIN_BOARD_SDA           (20)
#define PIN_BOARD_SCL           (21)
#define PIN_LED                 (0)
#define PIN_BUTTON              (9)

////////////////////////////////////////////////////////////////////////////////
// LoRa
// NOTE: no USE_SX1268 define here - the board auto-detects the chip
// (SX1262 / SX1268 / LLCC68) at boot; the wrapper headers included by
// target.h define USE_SX1262/USE_SX1268 so chip-specific code paths are
// compiled in for all three.

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
