/*
  Event.cpp - nightsky configuration.
*/

#include "Event.h"
#include "Arduino.h"
#include <ArduinoJson.h>
#include <time.h>
#include <ezTime.h>

#include "Config.h"

#define ARRAYSIZE(x)  (sizeof(x) / sizeof(x[0]))

Event::Event(){
}

void Event::begin(Timezone tz, Config conf){
  _tz = tz;
  _conf = conf;
}

/**
 * finds next SchedItem for the active mode and returns a SchedItemEvent
 * setable is set to false if no SchedItem was found
 */
EventItem Event::nextEvent() {
  EventItem ei;
  if (_conf.mode == 1) {
    ei = nextEventOfArray(_conf.active, ARRAYSIZE(_conf.active));
  } else if(_conf.mode == 2) {
     ei = nextEventOfArray(_conf.relax, ARRAYSIZE(_conf.relax));
  }
  return ei;
}
/**
 * finds next SchedItem of a given mode array and returns a SchedItemEvent
 * setable is set to false if no SchedItem was found
 */
EventItem Event::nextEventOfArray(struct SchedItem arr[], size_t len) {
  EventItem ei;
  ei.setable = false;
  
  time_t wt; //waketime
  time_t st; //sleeptime
  time_t zt = 0; //the time

  //finds the closest event by comparing timestamps
  int i;
  for (i = 0; i < len; i++) { //loops on configs
    if (arr[i].wakeOn) {
     wt = _eventToTime(arr[i].day, arr[i].wakeTime);
      if(zt == 0 || zt > wt){
        zt = wt;
        ei.wake = true;
        ei.moon = arr[i].wakeMoon;
        ei.stars = arr[i].wakeStars;
      }
    }
    if (arr[i].sleepOn) {
      st = _eventToTime(arr[i].day, arr[i].sleepTime);
      if(zt == 0 || zt > st){
        zt = st;
        ei.wake = false;
        ei.moon = arr[i].sleepMoon;
        ei.stars = arr[i].sleepStars;
      }
    }
  }
  
  if(zt !=0) {
    ei.time = zt;
    ei.setable = true;
  }
  return ei;
  
}
/**
 * Converts an event to a time_t
 */
time_t Event::_eventToTime(uint8_t day, int time){
    uint8_t inDays = 0;
    time_t n = _tz.now();
    uint8_t wd = _tz.weekday();
    tmElements_t tm;
    breakTime(n, tm);
    
    //breaks time seconds into h,m,s
    uint32_t t = time;
    uint8_t s = t % 60;
    t = (t - s)/60;
    uint8_t m = t % 60;
    t = (t - m)/60;
    uint8_t h = t;

    //set event time to today
    time_t event = makeTime(h,m,s,tm.Day, tm.Month, tm.Year);

    //find in how many days the event is
    if(day < wd) {            // if prior to current day of week
      inDays = 7 - (wd - day);
    } else if( day > wd) {    // if in up coming days
      inDays = day - wd;
    } else {                  // if same day then compares time
      // if lesser event is in the past, then next week
      if (event < n) {
        inDays = 7;
      }
    }
    
    // moves n days in the future
    event = event + (inDays * 86400);
    return event;
}
