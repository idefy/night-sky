/*
  Config.h - nightsky configuration.
*/
#ifndef Config_h
#define Config_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include <time.h>
#include <SPIFFS.h>
#include "debug.h"

struct SchedItem
{
  uint8_t day; 
  bool wakeOn;
  int wakeTime;
  uint8_t wakeMoon;
  uint8_t wakeStars;
  bool sleepOn;
  int sleepTime;
  uint8_t sleepMoon;
  uint8_t sleepStars;
};

class Config
{
  public:
    Config();
    uint8_t mode;
    //uint8_t moonWake;
    uint8_t moonSleep;
    //uint8_t starsWake;
    uint8_t starsSleep;
    SchedItem active[7];
    SchedItem relax[7];
    void setDefaults();
    void fromJsonObject(DynamicJsonDocument doc);
    void fromJson(String input);
    String toJson();
    void load();
    void save();
    void reset();
    
  private:
    bool _isInit;
    static const int JSON_SIZE = 6144;
    static constexpr const char* const DFT_CONFIG_FILENAME = "/default.jsn";
    static constexpr const char* const CONFIG_FILENAME = "/config.jsn";
};

#endif
