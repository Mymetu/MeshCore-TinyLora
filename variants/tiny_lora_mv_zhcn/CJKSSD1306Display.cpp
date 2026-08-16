#include "CJKSSD1306Display.h"

#include <Arduino.h>
#include <OLEDDisplayFonts.h>
#include <SSD1306Wire.h>
#include <Wire.h>
#include <utf8_10x10.h>

CJKSSD1306Display::CJKSSD1306Display()
    : DisplayDriver(128, 64), _oled(nullptr), _isOn(false), _cursorX(0), _cursorY(0) {}

CJKSSD1306Display::~CJKSSD1306Display() {
  delete _oled;
}

bool CJKSSD1306Display::probe(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

bool CJKSSD1306Display::begin() {
  uint8_t address = DISPLAY_ADDRESS;
#ifdef DISPLAY_ADDRESS_FALLBACK
  if (!probe(address)) address = DISPLAY_ADDRESS_FALLBACK;
#endif
  if (!probe(address)) {
#ifdef MESH_DEBUG
    Serial.printf("DEBUG: CJK SSD1306 not detected at I2C address %02X\n", address);
#endif
    return false;
  }

  if (!_oled) {
    _oled = new SSD1306Wire(address, -1, -1, GEOMETRY_128_64, I2C_ONE,
                           DISPLAY_I2C_CLOCK);
  }
  if (!_oled || !_oled->init()) {
    delete _oled;
    _oled = nullptr;
    return false;
  }

  // The MV board's OLED is mounted opposite to this library's default orientation.
  _oled->flipScreenVertically();
  _oled->setTextAlignment(TEXT_ALIGN_LEFT);
  _oled->setUtf8Font(&utf8_10x10_font);
  applyTextSize(1);
  _isOn = true;
#ifdef MESH_DEBUG
  Serial.printf("DEBUG: CJK SSD1306 detected at I2C address %02X\n", address);
#endif
  return true;
}

void CJKSSD1306Display::turnOn() {
  if (!_oled) return;
  _oled->displayOn();
  _isOn = true;
}

void CJKSSD1306Display::turnOff() {
  if (!_oled) return;
  _oled->displayOff();
  _isOn = false;
}

void CJKSSD1306Display::clear() {
  if (!_oled) return;
  _oled->clear();
  _oled->display();
}

void CJKSSD1306Display::startFrame(Color bkg) {
  if (!_oled) return;
  _oled->clear();
  _oled->setTextAlignment(TEXT_ALIGN_LEFT);
  _oled->setColor(OLEDDISPLAY_COLOR::WHITE);
  applyTextSize(1);
  _cursorX = 0;
  _cursorY = 0;
  (void)bkg;
}

void CJKSSD1306Display::applyTextSize(int size) {
  if (!_oled) return;
  if (size >= 3) {
    _oled->setFont(ArialMT_Plain_24);
  } else if (size >= 2) {
    _oled->setFont(ArialMT_Plain_16);
  } else {
    _oled->setFont(ArialMT_Plain_10);
  }
  _oled->setUtf8Font(&utf8_10x10_font);
}

void CJKSSD1306Display::setTextSize(int size) {
  applyTextSize(size);
}

void CJKSSD1306Display::setColor(Color color) {
  if (!_oled) return;
  _oled->setColor(color == DARK ? OLEDDISPLAY_COLOR::BLACK : OLEDDISPLAY_COLOR::WHITE);
}

void CJKSSD1306Display::setCursor(int x, int y) {
  _cursorX = x;
  _cursorY = y;
}

void CJKSSD1306Display::print(const char* text) {
  if (!_oled || !text) return;
  _oled->setTextAlignment(TEXT_ALIGN_LEFT);
  _oled->drawString(_cursorX, _cursorY, String(text));
  _cursorX += getTextWidth(text);
}

void CJKSSD1306Display::printWordWrap(const char* text, int max_width) {
  if (!_oled || !text || max_width <= 0) return;
  _oled->setTextAlignment(TEXT_ALIGN_LEFT);
  _oled->drawStringMaxWidth(_cursorX, _cursorY, max_width, String(text));
}

void CJKSSD1306Display::fillRect(int x, int y, int w, int h) {
  if (_oled) _oled->fillRect(x, y, w, h);
}

void CJKSSD1306Display::drawRect(int x, int y, int w, int h) {
  if (_oled) _oled->drawRect(x, y, w, h);
}

void CJKSSD1306Display::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  if (_oled) _oled->drawXbm(x, y, w, h, bits);
}

uint16_t CJKSSD1306Display::getTextWidth(const char* text) {
  if (!_oled || !text) return 0;
  return _oled->getStringWidth(String(text));
}

size_t CJKSSD1306Display::utf8CharLength(uint8_t first_byte) {
  if ((first_byte & 0x80) == 0) return 1;
  if ((first_byte & 0xE0) == 0xC0) return 2;
  if ((first_byte & 0xF0) == 0xE0) return 3;
  if ((first_byte & 0xF8) == 0xF0) return 4;
  return 1;
}

void CJKSSD1306Display::drawTextEllipsized(int x, int y, int max_width, const char* text) {
  if (!_oled || !text || max_width <= 0) return;
  if (getTextWidth(text) <= max_width) {
    setCursor(x, y);
    print(text);
    return;
  }

  const char* ellipsis = "...";
  int available = max_width - getTextWidth(ellipsis);
  if (available <= 0) return;

  char output[256];
  size_t output_len = 0;
  size_t input_len = strlen(text);
  for (size_t pos = 0; pos < input_len;) {
    size_t char_len = utf8CharLength((uint8_t)text[pos]);
    if (pos + char_len > input_len || output_len + char_len >= sizeof(output) - 4) break;
    memcpy(output + output_len, text + pos, char_len);
    output_len += char_len;
    output[output_len] = 0;
    if (getTextWidth(output) > available) {
      output_len -= char_len;
      break;
    }
    pos += char_len;
  }
  memcpy(output + output_len, ellipsis, 4);
  setCursor(x, y);
  print(output);
}

size_t CJKSSD1306Display::copyCompleteUtf8(char* dest, const char* src, size_t dest_size) {
  if (!dest || dest_size == 0) return 0;
  if (!src) {
    dest[0] = 0;
    return 0;
  }

  size_t written = 0;
  size_t src_len = strlen(src);
  for (size_t pos = 0; pos < src_len;) {
    size_t char_len = utf8CharLength((uint8_t)src[pos]);
    if (pos + char_len > src_len || written + char_len >= dest_size) break;
    memcpy(dest + written, src + pos, char_len);
    written += char_len;
    pos += char_len;
  }
  dest[written] = 0;
  return written;
}

void CJKSSD1306Display::translateUTF8ToBlocks(char* dest, const char* src, size_t dest_size) {
  copyCompleteUtf8(dest, src, dest_size);
}

void CJKSSD1306Display::endFrame() {
  if (_oled) _oled->display();
}
