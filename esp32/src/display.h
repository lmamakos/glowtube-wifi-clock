#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>

#define SELECT_NONE 0x81
#define SELECT_ALL 0x80
#define MAX_UPDATES 2

typedef void (*OnDigitColorChanged)(uint8_t digit, uint32_t color);

typedef struct
{
    uint8_t *buffer;
    size_t bufferSize;
    uint8_t selectLcds;
    uint32_t color;
} LcdUpdate;

class Display
{
public:
    Display(FS &fs);
    void begin();
    void drawTime(uint32_t time);
    void onDigitColorChanged(OnDigitColorChanged colorChanged);
    void invalidateDigitCache();
    void selectLcd(uint8_t index);
    TFT_eSPI tft;

private:
    void loadUpdate(uint8_t digit, LcdUpdate &update);
    void executeUpdate(LcdUpdate &update);

    FS &fs;
    OnDigitColorChanged colorChanged;
    LcdUpdate updates[MAX_UPDATES];
    uint8_t lastDigits[6];
};

#endif