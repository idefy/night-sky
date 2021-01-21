/*
  Config.cpp - nightsky configuration.
*/

#include "Arduino.h"
#include <ArduinoJson.h>
#include "Config.h"
//#define USE_LittleFS

#include <FS.h>
#ifdef USE_LittleFS
  #define SPIFFS LITTLEFS
  #include <LITTLEFS.h> 
#else
  #include <SPIFFS.h>
#endif 

#define ARRAYSIZE(x)  (sizeof(x) / sizeof(x[0]))

Config::Config()
{
  _isInit = false;
}

/**
 * Gives a default config and sets _isInit to true
 */
void Config::setDefaults(){
  long h = 3600000;
  moonWake = 30;
  moonSleep = 30;
  starsWake = 45;
  starsSleep = 45;
  
  active[0] = (SchedItem){true, (7*h), true, true, true, true, true, true, false, false};
  active[1] = (SchedItem){true, (21*h), false, true, true, true, true, true, false, false};
  active[2] = (SchedItem){true, (9*h), true, false, false, false, false, false, true, true};
  active[3] = (SchedItem){true, (23*h), false, false, false, false, false, false, true, true};
  relax[0] = (SchedItem){true, (9*h), true, true, true, true, true, true, false, false};
  relax[1] = (SchedItem){true, (22*h), false, true, true, true, true, true, false, false};
  relax[2] = (SchedItem){true, (10*h), true, false, false, false, false, false, true, true};
  relax[3] = (SchedItem){true, (23*h), false, false, false, false, false, false, true, true};
  
  _isInit = true;
}

void Config::reset() {
  Serial.print("reseting config");
  setDefaults();
  save();
}

void Config::load() {
  Serial.print("loading config...");
  File file = SPIFFS.open(CONFIG_FILENAME);
  DynamicJsonDocument doc(JSON_SIZE);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println(F("Failed to read file, using default configuration"));
    setDefaults();
    save();
  } else {
    fromJsonObject(doc);
    Serial.println(" ...loading completed");
  }
   file.close();
}

void Config::save(){
  SPIFFS.remove(CONFIG_FILENAME);
  //Serial.println(CONFIG_FILENAME);
  // Open file for writing
  File file = SPIFFS.open(CONFIG_FILENAME, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  } else {
    String s = toJson();
    Serial.println("config being saved:");
    Serial.println(s);
    if (file.print(s)) {
      Serial.println("File was written");
    } else {
      Serial.println("File write failed");
    }
    //file.println(s);
    file.close();
  }
}
/**
 * reads a JSON string to update Config
 */
void Config::fromJson(String input)
{
  DynamicJsonDocument doc(JSON_SIZE);
  deserializeJson(doc, input);
  fromJsonObject(doc);
}

void Config::fromJsonObject(DynamicJsonDocument doc){
  moonWake = doc["moonWake"]; // 30
  moonSleep = doc["moonSleep"]; // 30
  starsWake = doc["starsWake"]; // 45
  starsSleep = doc["starsSleep"]; // 45
  int i_a = 0;
  
  for (JsonObject elem : doc["active"].as<JsonArray>()) {
    bool isOn = elem["isOn"]; // true
    long sTime = elem["sTime"];
    bool onWake = elem["onWake"]; // true
    bool mo = elem["mo"]; // true
    bool tu = elem["tu"]; // true
    bool we = elem["we"]; // true
    bool th = elem["th"]; // true
    bool fr = elem["fr"]; // true
    bool sa = elem["sa"]; // false
    bool su = elem["su"]; // false
    active[i_a] = (SchedItem){isOn, sTime, onWake, mo, tu, we, th, fr, sa, su};
    i_a++;
  }
  
  int i_r = 0;
  for (JsonObject elem : doc["relax"].as<JsonArray>()) {
    bool isOn = elem["isOn"]; // true
    long sTime =  elem["sTime"];
    bool onWake = elem["onWake"]; // true
    bool mo = elem["mo"]; // true
    bool tu = elem["tu"]; // true
    bool we = elem["we"]; // true
    bool th = elem["th"]; // true
    bool fr = elem["fr"]; // true
    bool sa = elem["sa"]; // false
    bool su = elem["su"]; // false
    relax[i_r] = (SchedItem){isOn, sTime, onWake, mo, tu, we, th, fr, sa, su};
    i_r++;
  }

  _isInit = true;
}

/**
 * serializes config to JSON format
 */
String Config::toJson()
{
  if( !_isInit) setDefaults();
  
  DynamicJsonDocument doc(JSON_SIZE);

  doc["moonWake"] = moonWake;
  doc["moonSleep"] = moonSleep;
  doc["starsWake"] = starsWake;
  doc["starsSleep"] = starsSleep;

  JsonArray activeJSON = doc.createNestedArray("active");
  int i;
  for(i = 0; i < ARRAYSIZE(active); i++){
    JsonObject activeJSON_ = activeJSON.createNestedObject();
    activeJSON_["isOn"] = active[i].isOn;
    //activeJSON_["sTime"] = 0;
    activeJSON_["sTime"] = active[i].sTime;
    activeJSON_["onWake"] = active[i].onWake;
    activeJSON_["mo"] = active[i].mo;
    activeJSON_["tu"] = active[i].tu;
    activeJSON_["we"] = active[i].we;
    activeJSON_["th"] = active[i].th;
    activeJSON_["fr"] = active[i].fr;
    activeJSON_["sa"] = active[i].sa;
    activeJSON_["su"] = active[i].su;
  }
 
  
  JsonArray relaxJSON = doc.createNestedArray("relax");
  for(i = 0; i < ARRAYSIZE(relax); i++){
    JsonObject relaxJSON_ = relaxJSON.createNestedObject();
    relaxJSON_["isOn"] = relax[i].isOn;
    relaxJSON_["sTime"] = 0;
    //relaxJSON_["sTime"] = relax[i].sTime;
    relaxJSON_["onWake"] = relax[i].onWake;
    relaxJSON_["mo"] = relax[i].mo;
    relaxJSON_["tu"] = relax[i].tu;
    relaxJSON_["we"] = relax[i].we;
    relaxJSON_["th"] = relax[i].th;
    relaxJSON_["fr"] = relax[i].fr;
    relaxJSON_["sa"] = relax[i].sa;
    relaxJSON_["su"] = relax[i].su;
  }
  
  String output;
  serializeJson(doc, output);
  //Serial.println(output);
  return output;
}
