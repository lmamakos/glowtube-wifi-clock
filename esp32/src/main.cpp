#include <Arduino.h>
#include "hal.h"
// #include <WiFi.h>
#include <FastLED.h>
// #include <ESPmDNS.h>
#include <SPIFFS.h>

#include "display.h"
#include "hal.h"
#include "clock.h"
// #include "web.h"

#define USE_FS SPIFFS

Display display(USE_FS);

WifiClock wificlock(display, USE_FS);
// Web web(USE_FS, wificlock);

CRGB leds[NUM_LEDS];

void onDigitColorChanged(uint8_t digit, uint32_t color)
{
  for (uint8_t i = 0; i < 6; i++)
  {
    if (digit & (1 << i))
      leds[i] = color;
  }
  FastLED.show();
}

void onDigitsChanged()
{
  display.invalidateDigitCache();
}

void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, BACKLIGHT_PIN>(leds, NUM_LEDS);
  fill_solid(leds, NUM_LEDS, CRGB::DarkOrange);
  FastLED.show();

#if 0
  MDNS.begin("wifi-clock");
  MDNS.addService("http", "tcp", 80);

  WiFi.begin();
#endif
  display.onDigitColorChanged(onDigitColorChanged);
  USE_FS.begin();
  display.begin();
  // wificlock.begin();
#if 0
  web.onDigitsChanged(onDigitsChanged);
  web.begin();
#endif
  Serial.print("end setup()\n");
}

void loop()
{
  static u_int32_t counter = 0;

  EVERY_N_MILLISECONDS(1000) {
    Serial.printf("time %u\n", counter);
    display.drawTime(counter++);
  }

}
