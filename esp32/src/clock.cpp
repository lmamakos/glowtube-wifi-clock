#include "clock.h"

WifiClock *clockInstance;

TimeChangeRule rule;

// RTC_DS3231 RTC;
RTC_Millis RTC;   // just use software millisecond counter for now

WifiClock::WifiClock(Display &display, FS &fs)
    : display(display),
      timeClient(ntpUDP),
      timezone(rule),
      fs(fs)
{
    _ntpEnabled = false;
    _wifi_ssid = NULL;
    _wifi_password = NULL;
}

void WifiClock::begin()
{
    clockInstance = this;
    loadConfig(true);

    if (_wifi_ssid && _wifi_password) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(_wifi_ssid, _wifi_password);
        Serial.printf("Connecting to WiFi network %s %s ", _wifi_ssid, _wifi_password);
        display.tft.printf("Conncting to WiFi network '%s' ", _wifi_ssid);
    }
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        display.tft.print(".");
        delay(1000);
    }
    Serial.print("\nConnected. IP addr: "); Serial.println(WiFi.localIP());
    display.tft.println("  Connected!  IP address:\n");
    display.tft.println(WiFi.localIP());

    displayTime.attach(.2, []
                       { clockInstance->updateDisplay(); });

    sync();     // launch an NTP sync request

  //  setSyncProvider([]
  //              { return RTC.get(); });
}

void WifiClock::sync()
{
    if (_ntpEnabled && WiFi.status() == WL_CONNECTED && timeClient.update())
    {
        auto ntpTime = timeClient.getEpochTime();
        // RTC.set(ntpTime);
        setTime(ntpTime);
        Serial.print("NTP time set\n");
    }
}

void WifiClock::updateDisplay()
{
    auto localTime = getLocalTime();
    tmElements_t timeElements;
    breakTime(localTime, timeElements);

    static uint8_t lastSecond = 0xFF;
    if (timeElements.Second != lastSecond)
    {
        lastSecond = timeElements.Second;
        auto time = timeElements.Hour * 10000 + timeElements.Minute * 100 + timeElements.Second;
        display.drawTime(time);
    }
}

void WifiClock::loadConfig(bool first)
{
    if (!fs.exists("/config.json"))
    {
        return;
    }

    auto file = fs.open("/config.json", "r");
    size_t fileSize = file.size();
    uint8_t *json = (uint8_t *)malloc(fileSize + 1);
    file.read(json, fileSize);
    json[fileSize] = 0;

    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(json);

    JsonObject &wifi = root["wifi"];
    _wifi_ssid = wifi["ssid"];
    _wifi_password = wifi["password"];
    Serial.printf("config wifi=%s password=%s\n", _wifi_ssid, _wifi_password);

    JsonObject &ntp = root["ntp"];
    _ntpEnabled = ntp["enable"];
    if (_ntpEnabled)
    {
        static String ntpServer = ntp["server"];
        timeClient = NTPClient(ntpUDP, ntpServer.c_str(), 0, 1000);
        int updateInterval = ntp["updateInterval"];
        updateTime.attach(updateInterval, []
                          { clockInstance->sync(); });

        if (!first)
        {
            sync();
        }
    }
    else
    {
        updateTime.detach();
    }

    JsonObject &tz = root["timezone"];
    bool enableDst = tz["dstEnable"];
    if (!enableDst)
    {
        TimeChangeRule rule;
        rule.offset = tz["offset"];
        timezone.setRules(rule, rule);
    }
    else
    {
        TimeChangeRule start, end;
        parseTimezoneRule(tz["dstStart"], start);
        parseTimezoneRule(tz["dstEnd"], end);
        timezone.setRules(start, end);
    }

    if (!first)
    {
        updateDisplay();
    }
}

void WifiClock::parseTimezoneRule(JsonObject &root, TimeChangeRule &rule)
{
    rule.week = root["week"];
    rule.dow = root["dow"];
    rule.month = root["month"];
    rule.hour = root["hour"];
    rule.offset = root["offset"];
}
