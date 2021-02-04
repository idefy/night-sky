/*
  Event.h - nightsky configuration.
*/
#ifndef Event_h
#define Event_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include <time.h>
#include <ezTime.h>

#include "Config.h"

#include "debug.h"

struct EventItem
{
  bool setable;
  time_t time;
  bool wake;
  uint8_t moon;
  uint8_t stars;
};

class Event
{
  public:
    Event();
    void begin(Timezone tz, Config conf);
    EventItem nextEvent();
  private:
    Timezone _tz;
    Config _conf;
    time_t _eventToTime(uint8_t day, int time);
    EventItem nextEventOfArray(struct SchedItem arr[], size_t len);
};

#endif
