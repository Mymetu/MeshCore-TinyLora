#pragma once

#include <helpers/ui/DisplayDriver.h>

class SSD1306Wire;

#ifndef DISPLAY_ADDRESS
  #define DISPLAY_ADDRESS 0x3C
#endif

#ifndef DISPLAY_I2C_CLOCK
  #define DISPLAY_I2C_CLOCK 400000
#endif

class CJKSSD1306Display : public DisplayDriver {
  SSD1306Wire* _oled;
  bool _isOn;
  int _cursorX;
  int _cursorY;

  bool probe(uint8_t address);
  void applyTextSize(int size);
  static size_t utf8CharLength(uint8_t first_byte);
  static size_t copyCompleteUtf8(char* dest, const char* src, size_t dest_size);

public:
  CJKSSD1306Display();
  ~CJKSSD1306Display();

  bool begin();
  bool isOn() override { return _isOn; }
  void turnOn() override;
  void turnOff() override;
  void clear() override;
  void startFrame(Color bkg = DARK) override;
  void setTextSize(int size) override;
  void setColor(Color color) override;
  void setCursor(int x, int y) override;
  void print(const char* text) override;
  void printWordWrap(const char* text, int max_width) override;
  void fillRect(int x, int y, int w, int h) override;
  void drawRect(int x, int y, int w, int h) override;
  void drawXbm(int x, int y, const uint8_t* bits, int w, int h) override;
  uint16_t getTextWidth(const char* text) override;
  void drawTextEllipsized(int x, int y, int max_width, const char* text) override;
  void translateUTF8ToBlocks(char* dest, const char* src, size_t dest_size) override;
  void endFrame() override;
};
