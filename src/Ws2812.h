#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

void ws2812Setup(void);
void ws2812Loop(void);

void ws2812Demo(void);

void ledSetState(String);
void fullColorWipe(bool mode);
void colorWipe(uint32_t c, uint8_t wait);

void setLed(uint16_t i, String color);
String getLedRgbColor(uint16_t i);

extern Adafruit_NeoPixel strip;