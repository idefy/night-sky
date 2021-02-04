/*
  Config.cpp - nightsky configuration.
*/

#include "Arduino.h"
#include <ArduinoJson.h>
#include <ezTime.h>
#include <time.h>

#include "Config.h"
#include <SPIFFS.h>

#define ARRAYSIZE(x)  (sizeof(x) / sizeof(x[0]))

Config::Config()
{
  _isInit = false;
}

/**
   Gives a default config and sets _isInit to true
*/
void Config::setDefaults() {
  DEBUG_PRINT("Loading DEFAULT config...");
  File file = SPIFFS.open(DFT_CONFIG_FILENAME);
  DynamicJsonDocument doc(JSON_SIZE);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    DEBUG_PRINTLN(F("Failed to read file, using default configuration"));  
  } else {
    fromJsonObject(doc);
    save();
    DEBUG_PRINTLN(" ...loading completed");
  }
  file.close(); 
}

void Config::reset() {
  DEBUG_PRINT("reseting config");
  setDefaults();
  save();
}

void Config::load() {
  DEBUG_PRINT("loading config...");
  File file = SPIFFS.open(CONFIG_FILENAME);
  DynamicJsonDocument doc(JSON_SIZE);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    DEBUG_PRINTLN(F("Failed to read file, using default configuration"));
    setDefaults();
    save();
  } else {
    fromJsonObject(doc);
    DEBUG_PRINTLN(" ...loading completed");
  }
  file.close();
}

void Config::save() {
  SPIFFS.remove(CONFIG_FILENAME);
  //DEBUG_PRINTLN(CONFIG_FILENAME);
  // Open file for writing
  File file = SPIFFS.open(CONFIG_FILENAME, FILE_WRITE);
  if (!file) {
    DEBUG_PRINTLN(F("Failed to create file"));
    return;
  } else {
    String s = toJson();
    DEBUG_PRINTLN("config being saved:");
    DEBUG_PRINTLN(s);
    if (file.print(s)) {
      DEBUG_PRINTLN("File was written");
    } else {
      DEBUG_PRINTLN("File write failed");
    }
    //file.println(s);
    file.close();
  }
}
/**
   reads a JSON string to update Config
*/
void Config::fromJson(String input)
{
  DynamicJsonDocument doc(JSON_SIZE);
  deserializeJson(doc, input);
  fromJsonObject(doc);
}

void Config::fromJsonObject(DynamicJsonDocument doc) {
  moonSleep = doc["moonSleep"]; // 30
  starsSleep = doc["starsSleep"]; // 45
  mode = doc["mode"];
  int i_a = 0;

  for (JsonObject elem : doc["active"].as<JsonArray>()) {
    uint8_t day = elem["day"];
    bool wakeOn = elem["wakeOn"]; // true
    int wakeTime = elem["wakeTime"];
    uint8_t wakeMoon = elem["wakeMoon"];
    uint8_t wakeStars = elem["wakeStars"];
    bool sleepOn = elem["sleepOn"]; // true
    int sleepTime = elem["sleepTime"];
    uint8_t sleepMoon = elem["sleepMoon"];
    uint8_t sleepStars = elem["sleepStars"];

    active[i_a] = (SchedItem) {
      day, wakeOn, wakeTime, wakeMoon, wakeStars, sleepOn, sleepTime, sleepMoon, sleepStars
    };
    i_a++;
  }

  int i_r = 0;
  for (JsonObject elem : doc["relax"].as<JsonArray>()) {
    uint8_t day = elem["day"];
    bool wakeOn = elem["wakeOn"]; // true
    int wakeTime = elem["wakeTime"];
    uint8_t wakeMoon = elem["wakeMoon"];
    uint8_t wakeStars = elem["wakeStars"];
    bool sleepOn = elem["sleepOn"]; // true
    int sleepTime = elem["sleepTime"];
    uint8_t sleepMoon = elem["sleepMoon"];
    uint8_t sleepStars = elem["sleepStars"];
    
    relax[i_r] = (SchedItem) {
       day, wakeOn, wakeTime, wakeMoon, wakeStars, sleepOn, sleepTime, sleepMoon, sleepStars
    };
    i_r++;
  }

  _isInit = true;
}

/**
   serializes config to JSON format
*/
String Config::toJson()
{
  if ( !_isInit) setDefaults();

  DynamicJsonDocument doc(JSON_SIZE);

  doc["moonSleep"] = moonSleep;
  doc["starsSleep"] = starsSleep;
  doc["mode"]= mode;

  JsonArray activeJSON = doc.createNestedArray("active");
  int i;
  for (i = 0; i < ARRAYSIZE(active); i++) {
    JsonObject activeJSON_ = activeJSON.createNestedObject();
    activeJSON_["day"] = active[i].day;
    activeJSON_["wakeOn"] = active[i].wakeOn;
    activeJSON_["wakeTime"] = active[i].wakeTime;
    activeJSON_["wakeMoon"] = active[i].wakeMoon;
    activeJSON_["wakeStars"] = active[i].wakeStars;
    activeJSON_["sleepOn"] = active[i].sleepOn;
    activeJSON_["sleepTime"] = active[i].sleepTime;
    activeJSON_["sleepMoon"] = active[i].sleepMoon;
    activeJSON_["sleepStars"] = active[i].sleepStars;
  }

  JsonArray relaxJSON = doc.createNestedArray("relax");
  for (i = 0; i < ARRAYSIZE(relax); i++) {
    JsonObject relaxJSON_ = relaxJSON.createNestedObject();
    relaxJSON_["day"] = relax[i].day;
    relaxJSON_["wakeOn"] = relax[i].wakeOn;
    relaxJSON_["wakeTime"] = relax[i].wakeTime;
    relaxJSON_["wakeMoon"] = relax[i].wakeMoon;
    relaxJSON_["wakeStars"] = relax[i].wakeStars;
    relaxJSON_["sleepOn"] = relax[i].sleepOn;
    relaxJSON_["sleepTime"] = relax[i].sleepTime;
    relaxJSON_["sleepMoon"] = relax[i].sleepMoon;
    relaxJSON_["sleepStars"] = relax[i].sleepStars;
  }

  String output;
  serializeJson(doc, output);
  //DEBUG_PRINTLN(output);
  return output;
}
