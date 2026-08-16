/*
 * TinyLora MV ZHCN - ESP32-C3, SX1262, GPS and environmental sensors.
 * Screen, joystick and the MV-specific input peripherals are intentionally
 * not enabled in this first MeshCore adaptation.
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// I2C environmental sensors: AHT20 + BMP280
#define PIN_BOARD_SDA           (0)
#define PIN_BOARD_SCL           (1)

////////////////////////////////////////////////////////////////////////////////
// GPS: GPS TX -> ESP32-C3 GPIO21, GPS RX <- ESP32-C3 GPIO20
#define PIN_GPS_TX              (21)
#define PIN_GPS_RX              (20)
#define PIN_GPS_EN              (12)
#define PIN_GPS_EN_ACTIVE       HIGH

// MV buzzer control. This adaptation keeps it disabled rather than enabling
// the companion UI sound feature.
#define PIN_MV_BUZZER           (5)

////////////////////////////////////////////////////////////////////////////////
// Battery divider: two 1 Mohm resistors, battery sense on GPIO2
#define PIN_VBAT_READ           (2)
#define ADC_MULTIPLIER          (2.0F)
#define ADC_RESOLUTION          (12)
#define AREF_VOLTAGE            (3.0F)
#define BATTERY_SAMPLES         (8)

////////////////////////////////////////////////////////////////////////////////
// LoRa: TinyLora-C3 V2-compatible SX1262 with 1.8 V TCXO
#define USE_SX1262

#define P_LORA_NSS              (8)
#define P_LORA_RESET            (11)
#define P_LORA_DIO_0            (RADIOLIB_NC)
#define P_LORA_DIO_1            (3)
#define P_LORA_DIO_2            (RADIOLIB_NC)
#define P_LORA_BUSY             (4)
#define P_LORA_SCLK             (10)
#define P_LORA_MISO             (6)
#define P_LORA_MOSI             (7)

#define SX126X_DIO3_TCXO_VOLTAGE (1.8)
#define SX126X_DIO2_AS_RF_SWITCH (true)
#define SX126X_CURRENT_LIMIT     (140)
#define SX126X_RX_BOOSTED_GAIN   (1)
