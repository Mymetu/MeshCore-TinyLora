/*
 * variant.h
 *
 * TinyLora C3 V4/V5 - ESP32-C3 based board
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Bat
#define PIN_VBAT_READ           (1)
#define ADC_MULTIPLIER          (2.0F)
#define ADC_RESOLUTION          (12)
#define AREF_VOLTAGE            (3.0F)
#define BATTERY_SAMPLES         (8)

////////////////////////////////////////////////////////////////////////////////
// Other Pin
#define PIN_BOARD_SDA           (20)
#define PIN_BOARD_SCL           (21)
#define PIN_LED                 (0)

////////////////////////////////////////////////////////////////////////////////
// Lora
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
#define SX126X_RXEN              (2)
#define SX126X_TXEN              (RADIOLIB_NC)
#define SX126X_DIO3_TCXO_VOLTAGE (1.8)
#define SX126X_DIO2_AS_RF_SWITCH (true)
#define SX126X_CURRENT_LIMIT     (140)
#define SX126X_RX_BOOSTED_GAIN   (1)
