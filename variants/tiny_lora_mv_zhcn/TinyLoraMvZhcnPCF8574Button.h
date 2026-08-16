#pragma once

#include <Arduino.h>
#include <Wire.h>

#ifndef PCF8574_ADDRESS
#define PCF8574_ADDRESS 0x20
#endif

#ifndef BUTTON_EVENT_NONE
#define BUTTON_EVENT_NONE 0
#define BUTTON_EVENT_CLICK 1
#define BUTTON_EVENT_LONG_PRESS 2
#define BUTTON_EVENT_DOUBLE_CLICK 3
#define BUTTON_EVENT_TRIPLE_CLICK 4
#endif

class PCF8574Button {
  uint8_t _mask;
  uint16_t _long_press_ms;
  bool _multiclick;
  bool _raw_pressed;
  bool _previous_pressed;
  bool _cancel;
  uint32_t _raw_changed_at;
  uint32_t _down_at;
  uint32_t _last_click_at;
  uint8_t _click_count;
  bool _pending_click;

  static bool _bus_ready;
  static bool _cache_valid;
  static uint8_t _cached_state;
  static uint32_t _cached_at;
  static uint32_t _next_init_at;

  static void ensureBusReady();
  static uint8_t readState();
  bool isPressed(uint8_t state) const;

public:
  PCF8574Button(uint8_t mask, uint16_t long_press_ms = 0, bool multiclick = false);

  void begin();
  int check(bool repeat_click = false);
  void cancelClick();
  bool isPressed() const;
  uint8_t getPin() const { return _mask; }
};
