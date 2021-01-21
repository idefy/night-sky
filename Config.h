/*
  Config.h - nightsky configuration.
*/
#ifndef Config_h
#define Config_h

#include "Arduino.h"
#include <ArduinoJson.h>
//#define USE_LittleFS

#include <FS.h>
#ifdef USE_LittleFS
  #define SPIFFS LITTLEFS
  #include <LITTLEFS.h> 
#else
  #include <SPIFFS.h>
#endif 

struct SchedItem
{
  bool isOn; // true
  long sTime; // 1140000
  bool onWake; // true
  bool mo; // true
  bool tu; // true
  bool we; // true
  bool th; // true
  bool fr; // true
  bool sa; // false
  bool su; // false
};

class Config
{
  public:
    Config();
    int moonWake;
    int moonSleep;
    int starsWake;
    int starsSleep;
    SchedItem active[14];
    SchedItem relax[14];
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
    static constexpr const char* const CONFIG_FILENAME = "/config.jsn"; 
};

#endif
