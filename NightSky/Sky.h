/*
  Sky.h - nightsky GPIO interface manager.
*/

#ifndef Sky_h
#define Sky_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include "Config.h"
#include <Servo.h>
#include <time.h>
#include <ezTime.h>

#include "debug.h"

class Sky 
{
  public:
   Sky(byte moon, byte stars, byte servo);
   void setPhase(int angle);
   void setPhaseManual(bool on);
   bool getMoon();
   bool getStars();
   int getPhase();
   bool getPhaseManual();
   void lightCycle();
   void buttonListener(byte pin);
   void setConfig(Config conf);
   void startMoon(uint16_t seconds, bool wake = false);
   void endMoon();
   void startStars(uint16_t seconds);
   void endStars();
   void updateStarsMoon();
   String statusToJson();

 private:
   byte _moonPin;
   byte _starsPin;
   byte _servoPin;
   bool _moonOn;
   bool _starsOn;
   int _phaseAngle = 0;
   bool _phaseManualOn;
   bool _running;
   uint16_t _starsDur;
   uint16_t _moonDur;
   bool _moonOpenDir;
   time_t _start;
   Servo _servoObj;
   const int SHORT_PRESS_TIME = 500; // 500 milliseconds
   const int LONG_PRESS_TIME = 1000; // 1000 milliseconds
   int _bLastState = LOW;  // the previous state from the input pin
   int _bCurrentState;     // the current reading from the input pin
   unsigned long _bPressedTime  = 0;
   unsigned long _bReleasedTime = 0;
   Config _conf;
   void _setMoon(bool isOn);
   void _setStars(bool on);
   bool _moving;
};

#endif
