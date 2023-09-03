#ifndef _HAL_H_
#define _HAL_H_

// These pin definitions (and those in platformio.ini) correspond to the
// PCBWAY GlowTube clock.  Which seems to be similar to the original ElksTube
// configuration

// LCD shift register for chip select pins
#define SR_DATA_PIN (14)
#define SR_CLOCK_PIN (16)
#define SR_LATCH_PIN (17)

#define TFT_ENABLE_PIN (27)

// WS2812B RGB LEDs
#define BACKLIGHT_PIN (12)
#define NUM_LEDS 6

#define RTC_SCL_PIN   (21)
#define RTC_SDA_PIN   (22)

#define BUTTON_LEFT_PIN  (33)
#define BUTTON_MODE_PIN  (32)
#define BUTTON_RIGHT_PIN (35)
#define BUTTON_POWER_PIN (34)


#endif