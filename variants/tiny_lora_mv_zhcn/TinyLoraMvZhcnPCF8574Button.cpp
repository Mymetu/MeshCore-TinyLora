#include "TinyLoraMvZhcnPCF8574Button.h"

namespace {
constexpr uint32_t MULTI_CLICK_WINDOW_MS = 280;
constexpr uint32_t DEBOUNCE_MS = 40;
constexpr uint32_t IDLE_POLL_MS = 100;
constexpr uint32_t PRESSED_POLL_MS = 50;
constexpr uint32_t INIT_RETRY_MS = 1000;
}

bool PCF8574Button::_bus_ready = false;
bool PCF8574Button::_cache_valid = false;
uint8_t PCF8574Button::_cached_state = 0xFF;
uint32_t PCF8574Button::_cached_at = 0;
uint32_t PCF8574Button::_next_init_at = 0;

PCF8574Button::PCF8574Button(uint8_t mask, uint16_t long_press_ms, bool multiclick)
    : _mask(mask),
      _long_press_ms(long_press_ms),
      _multiclick(multiclick),
      _raw_pressed(false),
      _previous_pressed(false),
      _cancel(false),
      _raw_changed_at(0),
      _down_at(0),
      _last_click_at(0),
      _click_count(0),
      _pending_click(false) {}

void PCF8574Button::ensureBusReady() {
  if (_bus_ready) return;

  uint32_t now = millis();
  if (_next_init_at != 0 && (int32_t)(now - _next_init_at) < 0) return;
  _next_init_at = now + INIT_RETRY_MS;

  Wire.beginTransmission(PCF8574_ADDRESS);
  if (Wire.endTransmission() != 0) {
#ifdef MESH_DEBUG
    Serial.printf("DEBUG: PCF8574 not detected at I2C address %02X\n", PCF8574_ADDRESS);
#endif
    return;
  }

  // PCF8574 pins must be written HIGH before they can be used as inputs.
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(0xFF);
  if (Wire.endTransmission() == 0) {
    _bus_ready = true;
    _cache_valid = false;
    _next_init_at = 0;
#if defined(PCF8574_INT_PIN) && PCF8574_INT_PIN >= 0
    pinMode(PCF8574_INT_PIN, INPUT_PULLUP);
#endif
#ifdef MESH_DEBUG
    Serial.printf("DEBUG: PCF8574 detected at I2C address %02X\n", PCF8574_ADDRESS);
#endif
  }
}

void PCF8574Button::begin() {
  ensureBusReady();
}

uint8_t PCF8574Button::readState() {
  uint32_t now = millis();
  if (_cache_valid && _cached_at == now) return _cached_state;

  ensureBusReady();
  if (!_bus_ready) {
    _cached_state = 0xFF;
    return _cached_state;
  }

  if (_cache_valid) {
    uint32_t poll_interval = _cached_state == 0xFF ? IDLE_POLL_MS : PRESSED_POLL_MS;
#if defined(PCF8574_INT_PIN) && PCF8574_INT_PIN >= 0
    bool input_changed = digitalRead(PCF8574_INT_PIN) == LOW;
    if (!input_changed && (uint32_t)(now - _cached_at) < poll_interval) {
      return _cached_state;
    }
#else
    if ((uint32_t)(now - _cached_at) < poll_interval) return _cached_state;
#endif
  }

  _cached_at = now;
  _cache_valid = true;
  uint8_t received = Wire.requestFrom((uint8_t)PCF8574_ADDRESS, (uint8_t)1);
  if (received == 1 && Wire.available()) {
    _cached_state = Wire.read();
  }
  return _cached_state;
}

bool PCF8574Button::isPressed(uint8_t state) const {
  // The MV joystick switches pull PCF8574 inputs LOW when pressed.
  return (state & _mask) != _mask;
}

bool PCF8574Button::isPressed() const {
  return isPressed(readState());
}

void PCF8574Button::cancelClick() {
  _cancel = true;
  _down_at = 0;
  _click_count = 0;
  _last_click_at = 0;
  _pending_click = false;
}

int PCF8574Button::check(bool repeat_click) {
  uint32_t now = millis();
  bool raw_pressed = isPressed(readState());
  int event = BUTTON_EVENT_NONE;

  if (raw_pressed != _raw_pressed) {
    _raw_pressed = raw_pressed;
    _raw_changed_at = now;
  }

  if (_raw_pressed != _previous_pressed &&
      (uint32_t)(now - _raw_changed_at) >= DEBOUNCE_MS) {
    if (_raw_pressed) {
      _down_at = now;
    } else {
      if (_long_press_ms == 0 || (uint32_t)(now - _down_at) < _long_press_ms) {
        _click_count++;
        _last_click_at = now;
        _pending_click = true;
      }
      _down_at = 0;
    }
    _previous_pressed = _raw_pressed;
  }
  bool pressed = _previous_pressed;

  if (!pressed && _cancel) {
    _cancel = false;
  }

  if (_long_press_ms > 0 && _down_at != 0 &&
      (uint32_t)(now - _down_at) >= _long_press_ms) {
    if (_pending_click) {
      cancelClick();
    } else {
      event = BUTTON_EVENT_LONG_PRESS;
      _down_at = 0;
      _click_count = 0;
      _last_click_at = 0;
      _pending_click = false;
    }
  }

  if (pressed && repeat_click && _down_at != 0 &&
      (uint32_t)(now - _down_at) >= 700) {
    event = BUTTON_EVENT_CLICK;
  }

  uint32_t click_window = _multiclick ? MULTI_CLICK_WINDOW_MS : 0;
  if (_pending_click && !pressed &&
      (uint32_t)(now - _last_click_at) >= click_window) {
    if (!_cancel) {
      switch (_click_count) {
        case 1:
          event = BUTTON_EVENT_CLICK;
          break;
        case 2:
          event = BUTTON_EVENT_DOUBLE_CLICK;
          break;
        default:
          event = BUTTON_EVENT_TRIPLE_CLICK;
          break;
      }
    }
    _click_count = 0;
    _last_click_at = 0;
    _pending_click = false;
  }

  return event;
}
